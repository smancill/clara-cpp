/*
 * Copyright (c) 2016.  Jefferson Lab (JLab). All rights reserved. Permission
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

#include "service_engine.hpp"

#include "data_utils.hpp"
#include "logging.hpp"
#include "service_config.hpp"
#include "service_report.hpp"

#include <chrono>
#include <stdexcept>


namespace {

auto build_error_data(const char* msg,
                      int severity,
                      const std::exception& ex) -> clara::EngineData
{
    auto data = clara::EngineData{};
    data.set_data(clara::type::STRING.mime_type(), msg);
    data.set_description(ex.what());
    data.set_status(clara::EngineStatus::ERROR, severity);
    return data;
}


auto build_done_data(clara::EngineDataAccessor& accessor,
                     clara::EngineData& output) -> clara::EngineData
{
    auto done_output = clara::EngineData{};
    done_output.set_data(clara::type::STRING.mime_type(), "done");

    auto* done_meta = accessor.view_meta(done_output);
    auto* out_meta = accessor.view_meta(output);
    done_meta->CopyFrom(*out_meta);

    return done_output;
}

} // end namespace


namespace clara {

ServiceEngine::ServiceEngine(const Component& self,
                             const Component& frontend,
                             Engine* engine,
                             ServiceReport* report,
                             ServiceConfig* config)
  : Base{self, frontend}
  , engine_{engine}
  , report_{report}
  , config_{config}
  , input_types_{engine_->input_data_types()}
  , output_types_{engine_->output_data_types()}
  , compiler_{self.name()}
{
    // nop
}


ServiceEngine::~ServiceEngine() = default;


void ServiceEngine::setup(msg::Message& msg)
{
    auto parser = util::RequestParser::build(msg);
    auto report = parser.next_string();
    auto value = parser.next_integer();
    auto status = msg::proto::Meta::INFO;
    if (report == constants::service_report_done) {
        config_->set_done_count_threshold(value);
        config_->reset_done_count();
    } else if (report == constants::service_report_data) {
        config_->set_data_count_threshold(value);
        config_->reset_data_count();
    } else {
        status = msg::proto::Meta::ERROR;
        LOGGER->error("Invalid report request = %s", report);
    }
    if (msg.has_replyto()) {
        Base::send_response(msg, parser.request(), status);
    }
}


void ServiceEngine::configure(msg::Message& msg)
{
    auto input_data = get_engine_data(msg);
    auto output_data = configure_engine(input_data);

    update_metadata(input_data, output_data);

    if (msg.has_replyto()) {
        send_response(output_data, msg.replyto());
    } else {
        report_problem(output_data);
    }
}


void ServiceEngine::execute(msg::Message& msg)
{
    report_->add_n_requests();
    config_->add_request();

    auto input_data = get_engine_data(msg);
    parse_composition(input_data);
    auto output_data = execute_engine(input_data);

    update_metadata(input_data, output_data);

    if (msg.has_replyto()) {
        send_response(output_data, msg.replyto());
        return;
    }

    report_problem(output_data);
    if (output_data.status() == EngineStatus::ERROR) {
        report_->add_n_failures();
        return;
    }
    report_result(output_data);
    send_result(output_data, get_links(input_data, output_data));
}


auto ServiceEngine::configure_engine(EngineData& input) -> EngineData
{
    try {
        auto output_data = engine_->configure(input);
        if (!output_data.has_data()) {
            output_data.set_data(type::STRING.mime_type(), "done");
        }
        return output_data;
    } catch (const std::exception& e) {
        return build_error_data("unhandled exception", 4, e);
    }
}


auto ServiceEngine::execute_engine(EngineData& input) -> EngineData
{
    try {
        auto t0 = std::chrono::high_resolution_clock::now();
        auto output_data = engine_->execute(input);
        auto t1 = std::chrono::high_resolution_clock::now();

        if (!output_data.has_data()) {
            if (output_data.status() == EngineStatus::ERROR) {
                output_data.set_data(type::STRING.mime_type(), "udf");
            } else {
                throw std::runtime_error{"no output data"};
            }
        }

        auto d = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        report_->add_exec_time(d.count());
        accessor_.view_meta(output_data)->set_executiontime(d.count());

        return output_data;
    } catch (const std::exception& e) {
        return build_error_data("unhandled exception", 4, e);
    }
}


auto ServiceEngine::get_engine_data(msg::Message& msg) -> EngineData
{
    report_->add_bytes_recv(static_cast<std::int64_t>(msg.data().size()));
    return accessor_.deserialize(msg, input_types_);
}


auto ServiceEngine::put_engine_data(const EngineData& output,
                                    const msg::Topic& topic) -> msg::Message
{
    auto msg = accessor_.serialize(output, topic, output_types_);
    report_->add_bytes_sent(static_cast<std::int64_t>(msg.data().size()));
    return msg;
}


auto ServiceEngine::put_engine_data(const EngineData& output,
                                    const std::string& receiver) -> msg::Message
{
    auto topic = msg::Topic::raw(receiver);
    auto msg = accessor_.serialize(output, topic, output_types_);
    report_->add_bytes_sent(static_cast<std::int64_t>(msg.data().size()));
    return msg;
}


void ServiceEngine::update_metadata(const EngineData& input, EngineData& output)
{
    const auto* in_meta = accessor_.view_meta(input);
    auto* out_meta = accessor_.view_meta(output);

    out_meta->set_author(name());
    out_meta->set_version(engine_->version());

    if (!out_meta->has_communicationid()) {
        out_meta->set_communicationid(in_meta->communicationid());
    }
    out_meta->set_composition(in_meta->composition());
    out_meta->set_action(in_meta->action());
}


void ServiceEngine::parse_composition(const EngineData& input)
{
    const auto& current_composition = input.composition();
    if (current_composition != prev_composition_) {
        compiler_.compile(current_composition);
        prev_composition_ = current_composition;
    }
}


auto ServiceEngine::get_links(const EngineData& /*input*/,
                              const EngineData& /*output*/) -> std::set<std::string>
{
    return compiler_.outputs();
}


void ServiceEngine::send_response(EngineData& output, const msg::Topic& topic)
{
    auto con = connect();
    auto msg = put_engine_data(output, topic);
    publish(con, msg);
}


void ServiceEngine::send_result(EngineData& output,
                                const std::set<std::string>& links)
{
    for (auto&& ss : links) {
        auto host = util::get_dpe_host(ss);
        auto port = util::get_dpe_port(ss);
        auto addr = msg::ProxyAddress{std::string{host}, port};
        auto con = connect(addr);
        auto msg = put_engine_data(output, ss);
        publish(con, msg);
    }
}


void ServiceEngine::report_problem(EngineData& output)
{
    auto status = output.status();
    if (status == EngineStatus::ERROR) {
        report(constants::error, output);
    } else if (status == EngineStatus::WARNING) {
        report(constants::warning, output);
    }
}


void ServiceEngine::report_result(EngineData& output)
{
    if (config_->data_count() == config_->data_count_threshold()) {
        report(constants::data, output);
        config_->reset_data_count();
    }

    if (config_->done_count() == config_->done_count_threshold()) {
        auto done_output = build_done_data(accessor_, output);
        report(constants::done, done_output);
        config_->reset_done_count();
    }
}


void ServiceEngine::report(std::string_view topic_prefix, EngineData& output)
{
    auto topic = msg::Topic::raw(std::string{topic_prefix} + ":" + name());
    auto msg = accessor_.serialize(output, topic, output_types_);
    auto con = connect(frontend().addr());
    publish(con, msg);
}

} // end namespace clara
