/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/stdlib/event_reader_service.hpp>

#include <clara/stdlib/json_utils.hpp>

#include "service_utils.hpp"

#include <iostream>
#include <mutex>
#include <sstream>


using namespace std::literals::string_view_literals;

namespace {

constexpr auto conf_action = "action"sv;
constexpr auto conf_filename = "file"sv;

constexpr auto conf_action_open = "open"sv;
constexpr auto conf_action_close = "close"sv;

constexpr auto conf_events_skip = "skip"sv;
constexpr auto conf_events_max = "max"sv;

constexpr auto request_next = "next"sv;
constexpr auto request_next_rec = "next-rec"sv;
constexpr auto request_order = "order"sv;
constexpr auto request_count = "count"sv;

constexpr auto no_file = "No open file"sv;
constexpr auto end_of_file = "End of file"sv;

constexpr auto eof_not_from_writer = 0;
constexpr auto eof_waiting_rec = -1;

} // namespace


namespace clara::stdlib {

class EventReaderService::Impl
{
public:
    Impl(EventReaderService* service);
    ~Impl();

public:
    void open_file(const json11::Json& config_data);
    void close_file(const json11::Json& config_data);

    auto has_file() -> bool { return service_->has_file(); }
    void close_file();

private:
    void set_limits(const json11::Json& config_data);
    auto get_value(const json11::Json& config_data, std::string_view key,
                   int def_val, int min_val, int max_val) -> int;

public:
    void get_next_event(const EngineData& input, EngineData& output);
    void get_file_byte_order(EngineData& output);
    void get_event_count(EngineData& output);

private:
    auto is_rec_request(const EngineData& input) -> bool;
    void return_next_event(EngineData& output);
    auto get_order(Endian endian) -> std::string;

public:
    void reset();

private:
    std::string file_name_;
    std::string open_error_ = std::string{no_file};

    int current_event_;
    int last_event_;
    int event_count_;

    std::set<long> processing_events_;
    int eof_request_count_;

private:
    EventReaderService* service_;

    std::mutex mutex_;
};


EventReaderService::EventReaderService()
  : impl_{std::make_unique<Impl>(this)}
{
    // nop
}

EventReaderService::~EventReaderService() = default;


EventReaderService::Impl::Impl(EventReaderService* service)
  : service_{service}
{
    // nop
}

EventReaderService::Impl::~Impl() = default;


auto EventReaderService::configure(EngineData& input) -> EngineData
{
    if (input.mime_type() == type::JSON) {
        try {
            auto data = parse_json(data_cast<std::string>(input));
            auto action = get_string(data, conf_action);
            if (action == conf_action_open) {
                impl_->open_file(data);
            } else if (action == conf_action_close) {
                impl_->close_file(data);
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
        std::cerr << name() << " config: wrong mime-type " << input.mime_type()
                  << std::endl;
    }
    return {};
}


void EventReaderService::Impl::open_file(const json11::Json& config_data)
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
        set_limits(config_data);
        std::cout << service_->name() << " opened file " << file_name_
                  << std::endl;
    } catch (const EventReaderError& e) {
        std::cerr << service_->name() << " could not open file " << e.what()
                  << std::endl;
        file_name_.clear();
    }
}


void EventReaderService::Impl::set_limits(const json11::Json& config_data)
{
    event_count_ = service_->read_event_count();
    int skip_events = get_value(config_data, conf_events_skip,
                                0, 0, event_count_);
    if (skip_events != 0) {
        std::cout << service_->name() << " config: skip first " << skip_events
                  << " events" << std::endl;
    }
    current_event_ = skip_events;

    int rem_events = event_count_ - skip_events;
    int max_events = get_value(config_data, conf_events_max,
                               rem_events, 0, rem_events);
    if (max_events != rem_events) {
        std::cout << service_->name() << " config: read first " << max_events
                  << " events" << std::endl;
    }
    last_event_ = skip_events + max_events;

    processing_events_.clear();
    eof_request_count_ = 0;
}


auto EventReaderService::Impl::get_value(const json11::Json& config_data,
                                         std::string_view key,
                                         int def_val,
                                         int min_val,
                                         int max_val) -> int
{
    auto json_val = config_data[key];
    if (!json_val.is_null()) {
        if (json_val.is_number()) {
            int value = json_val.int_value();
            if (value >= min_val && value <= max_val) {
                return value;
            }
        }
        std::cerr << service_->name() << " config: "
                << "invalid value for '" << key << "': " << json_val.dump()
                << std::endl;
    }
    return def_val;
}


void EventReaderService::Impl::close_file(const json11::Json& config_data)
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
}


void EventReaderService::Impl::close_file()
{
    service_->close_file();
    std::cout << service_->name() << " closed file " << file_name_ << std::endl;
}


auto EventReaderService::execute(EngineData& input) -> EngineData
{
    auto output = EngineData();

    const auto& dt = input.mime_type();
    if (dt == type::STRING) {
        const auto& request = data_cast<std::string>(input);
        if (request == request_next || request == request_next_rec) {
            impl_->get_next_event(input, output);
        } else if (request == request_order) {
            impl_->get_file_byte_order(output);
        } else if (request == request_count) {
            impl_->get_event_count(output);
        } else {
            util::set_error(output, "Wrong input data: " + request);
        }
    } else {
        util::set_error(output, "Wrong input type: " + dt);
    }

    return output;
}


inline auto EventReaderService::Impl::is_rec_request(const EngineData& input) -> bool
{
    return data_cast<std::string>(input) == request_next_rec;
}


void EventReaderService::Impl::get_next_event(const EngineData& input,
                                              EngineData& output)
{
    std::unique_lock<std::mutex> lock{mutex_};

    bool from_rec = is_rec_request(input);
    if (from_rec) {
        processing_events_.erase(input.communication_id());
    }
    if (!has_file()) {
        util::set_error(output, open_error_, 1);
    } else if (current_event_ < last_event_) {
        return_next_event(output);
    } else {
        util::set_error(output, end_of_file, 1);
        if (from_rec) {
            if (processing_events_.empty()) {
                eof_request_count_++;
                util::set_error(output, end_of_file, eof_request_count_ + 1);
                output.set_data(type::SFIXED32, eof_request_count_);
            } else {
                output.set_data(type::SFIXED32, eof_waiting_rec);
            }
        } else {
            output.set_data(type::SFIXED32, eof_not_from_writer);
        }
    }
}


void EventReaderService::Impl::return_next_event(EngineData& output)
{
    try {
        auto event_id = current_event_++;
        output.set_communication_id(event_id);

        auto event = service_->read_event(event_id);
        output.set_data(service_->get_data_type(), std::move(event));
        output.set_description("data");

        processing_events_.insert(event_id);
    } catch (const EventReaderError& e) {
        std::ostringstream msg;
        msg << "Error requesting event " << current_event_
            << " from file " << file_name_
            << "\n\n" << e.what();
        util::set_error(output, msg.str(), 1);
    }
}


void EventReaderService::Impl::get_file_byte_order(EngineData& output)
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        try {
            auto order = get_order(service_->read_byte_order());
            output.set_data(type::STRING, order);
            output.set_description("byte order");
        } catch (const EventReaderError& e) {
            std::ostringstream msg;
            msg << "Error requesting byte-order from file " << file_name_
                << "\n\n" << e.what();
            util::set_error(output, msg.str(), 1);
        }
    } else {
        util::set_error(output, open_error_, 1);
    }
}


auto EventReaderService::Impl::get_order(Endian endian) -> std::string
{
    switch (endian) {
        case Endian::Little:
            return "LITTLE_ENDIAN";
        case Endian::Big:
        default:
            return "BIG_ENDIAN";
    }
}


void EventReaderService::Impl::get_event_count(EngineData& output)
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        output.set_data(type::SFIXED32, event_count_);
        output.set_description("event count");
    } else {
        util::set_error(output, open_error_, 1);
    }
}


auto EventReaderService::execute_group(const std::vector<EngineData>& /*inputs*/)
    -> EngineData
{
    return {};
}


auto EventReaderService::input_data_types() const -> std::vector<EngineDataType>
{
    return {type::JSON, type::STRING};
}


auto EventReaderService::output_data_types() const -> std::vector<EngineDataType>
{
    return {get_data_type(), type::STRING, type::SFIXED32};
}


auto EventReaderService::states() const -> std::set<std::string>
{
    return {};
}


void EventReaderService::reset()
{
    impl_->reset();
}


void EventReaderService::Impl::reset()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (has_file()) {
        close_file();
    }
}

} // end namespace clara::stdlib
