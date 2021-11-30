/*
 * Copyright (c) 2017.  Jefferson Lab (JLab). All rights reserved. Permission
 * to use, copy, modify, and distribute  this software and its documentation for
 * educational, research, and not-for-profit purposes, without fee and without a
 * signed licensing agreement.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY,
 * PROVIDED HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * This software was developed under the United States Government license.
 * For more information contact author at gurjyan@jlab.org
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 */

#include <clara/stdlib/event_writer_service.hpp>

#include <clara/stdlib/json_utils.hpp>

#include "service_utils.hpp"

#include <iostream>
#include <mutex>
#include <sstream>


using namespace std::literals::string_view_literals;

namespace {

constexpr auto conf_action = "action"sv;
constexpr auto conf_filename = "file"sv;
constexpr auto conf_order = "order"sv;

constexpr auto conf_action_open = "open"sv;
constexpr auto conf_action_close = "close"sv;
constexpr auto conf_action_skip = "skip"sv;

constexpr auto output_next = "next-rec"sv;
constexpr auto event_skip = "skip"sv;

constexpr auto no_file = "No open file"sv;

} // namespace


namespace clara::stdlib {

class EventWriterService::Impl
{
public:
    Impl(EventWriterService* service);
    ~Impl();

public:
    void open_file(const json11::Json& config_data);
    void close_file(const json11::Json& config_data);

    auto has_file() -> bool { return service_->has_file(); }
    void close_file();
    void skip_all();

public:
    void write_event(const EngineData& input, EngineData& output);

public:
    void reset();

private:
    std::string file_name_;
    std::string open_error_ = std::string{no_file};

    bool skip_events_ = false;
    int event_counter_ = 0;

    EventWriterService* service_;

    std::mutex mutex_;
};


EventWriterService::EventWriterService()
  : impl_{std::make_unique<Impl>(this)}
{
    // nop
}

EventWriterService::~EventWriterService() = default;


EventWriterService::Impl::Impl(EventWriterService* service)
  : service_{service}
{
    // nop
}

EventWriterService::Impl::~Impl() = default;


auto EventWriterService::configure(EngineData& input) -> EngineData
{
    if (input.mime_type() == type::JSON) {
        try {
            auto data = parse_json(input);
            auto action = get_string(data, conf_action);
            if (action == conf_action_open) {
                impl_->open_file(data);
            } else if (action == conf_action_close) {
                impl_->close_file(data);
            } else if (action == conf_action_skip) {
                impl_->skip_all();
            } else {
                std::cerr << name() << " config: invalid \"" << conf_action
                          << "\" value: \"" << action << "\"" << std::endl;
            }
        } catch (const std::bad_any_cast& e) {
            std::cerr << name() << " config: " << "input data is not JSON"
                      << std::endl;
        } catch (const std::exception& e) {
            std::cerr << name() << " config: " << e.what() << std::endl;
        }
    } else {
        std::cerr << name() << " config: wrong mime-type: " << input.mime_type()
                  << std::endl;
    }
    return {};
}


void EventWriterService::Impl::open_file(const json11::Json& config_data)
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        close_file();
    }

    file_name_ = get_string(config_data, conf_filename);
    std::cout << service_->name() << " request to open file " << file_name_
              << std::endl;
    try {
        service_->open_file(file_name_, config_data);
        event_counter_ = 0;
        std::cout << service_->name() << " opened file " << file_name_
                  << std::endl;
    } catch (const EventWriterError& e) {
        std::cerr << service_->name() << " could not open file " << e.what()
                  << std::endl;
        file_name_.clear();
        event_counter_ = 0;
    }
}


void EventWriterService::Impl::close_file(const json11::Json& config_data)
{
    std::unique_lock<std::mutex> lock{mutex_};

    file_name_ = get_string(config_data, conf_filename);
    std::cout << service_->name() << " request to close file " << file_name_
              << std::endl;
    if (has_file()) {
        close_file();
    } else {
        std::cerr << service_->name() << " file " << file_name_ << " not open"
                  << std::endl;
    }
    open_error_ = no_file;
    file_name_.clear();
    event_counter_ = 0;
}


void EventWriterService::Impl::close_file()
{
    service_->close_file();
    std::cout << service_->name() << " closed file " << file_name_ << std::endl;
}


void EventWriterService::Impl::skip_all()
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        skip_events_ = true;
        std::cout << service_->name() << " skipping all events" << std::endl;
    } else {
        std::cout << service_->name() << " file " << file_name_
                  << " is already open" << std::endl;
    }
}


auto EventWriterService::execute(EngineData& input) -> EngineData
{
    auto output = EngineData();

    if (input.mime_type() == type::STRING) {
        util::set_error(output, "Wrong input type: " + input.mime_type());
        return output;
    }

    impl_->write_event(input, output);

    return output;
}


void EventWriterService::Impl::write_event(const EngineData& input,
                                           EngineData& output)
{
    if (skip_events_ || input.description() == event_skip) {
        output.set_data(type::STRING, output_next);
        output.set_description("event skipped");
        return;
    }

    auto lock = std::unique_lock<std::mutex>{mutex_};
    if (has_file()) {
        try {
            service_->write_event(input.data());
            event_counter_++;
            output.set_data(type::STRING, output_next);
            output.set_description("event saved");
        } catch (const EventWriterError& e) {
            auto msg = std::ostringstream{};
            msg << "Error saving event to file " << file_name_ << "\n\n"
                << e.what();
            util::set_error(output, msg.str());
        }
    } else {
        util::set_error(output, open_error_);
    }
}


auto EventWriterService::execute_group(const std::vector<EngineData>& /*inputs*/)
    -> EngineData
{
    return {};
}


auto EventWriterService::input_data_types() const
    -> std::vector<EngineDataType>
{
    return {get_data_type(), type::JSON};
}


auto EventWriterService::output_data_types() const
    -> std::vector<EngineDataType>
{
    return {type::STRING};
}


auto EventWriterService::states() const -> std::set<std::string>
{
    return {};
}


void EventWriterService::reset()
{
    impl_->reset();
}


void EventWriterService::Impl::reset()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (has_file()) {
        close_file();
    }
}


auto EventWriterService::parse_byte_order(const json11::Json& opts)
    -> EventWriterService::Endian
{
    if (has_key(opts, conf_order)) {
        auto byte_order = get_string(opts, conf_order);
        if (byte_order == "BIG_ENDIAN") {
            return Endian::Big;
        }
    }
    return Endian::Little;
}

} // end namespace clara::stdlib
