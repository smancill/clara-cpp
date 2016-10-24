/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set ts=8 sts=4 et sw=4 tw=80: */
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

#include <xmsg/util.h>

namespace clara {

ServiceEngine::ServiceEngine(const Component& self,
                             const Component& frontend,
                             Engine* engine)
  : Base{self, frontend}
  , engine_{engine}
  , input_types_{engine_->input_data_types()}
  , output_types_{engine_->output_data_types()}
  , compiler_{self.name()}
{
    // nop
}


ServiceEngine::~ServiceEngine()
{
    // nop
}


void ServiceEngine::setup(xmsg::Message&)
{
    LOGGER->info("engine setup not implemetend (service = %s)", name());
}


void ServiceEngine::configure(xmsg::Message& msg)
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


void ServiceEngine::execute(xmsg::Message& msg)
{
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
        return;
    }
    send_result(output_data, get_links(input_data, output_data));
}


EngineData ServiceEngine::configure_engine(EngineData& input)
{
    try {
        auto output_data = engine_->configure(input);
        return output_data;
    } catch (const std::exception& e) {
        return util::build_error_data("unhandled exception", 4, e);
    }
}


EngineData ServiceEngine::execute_engine(EngineData& input)
{
    try {
        auto output_data = engine_->execute(input);
        if (!output_data.has_data()) {
            if (output_data.status() == EngineStatus::ERROR) {
                output_data.set_data(type::STRING.mime_type(), "udf");
            } else {
                throw std::runtime_error{"no output data"};
            }
        }
        return output_data;
    } catch (const std::exception& e) {
        return util::build_error_data("unhandled exception", 4, e);
    }
}


EngineData ServiceEngine::get_engine_data(xmsg::Message& msg)
{
    return accessor_.deserialize(msg, input_types_);
}


xmsg::Message ServiceEngine::put_engine_data(EngineData& output,
                                             const xmsg::Topic& topic)
{
    return accessor_.serialize(output, topic, output_types_);
}


xmsg::Message ServiceEngine::put_engine_data(EngineData& output,
                                             const std::string& receiver)
{
    auto topic = xmsg::Topic::raw(receiver);
    return accessor_.serialize(output, topic, output_types_);
}


void ServiceEngine::update_metadata(const EngineData& input, EngineData& output)
{
    auto in_meta = accessor_.view_meta(input);
    auto out_meta = accessor_.view_meta(output);

    out_meta->set_author(name());
    out_meta->set_version(engine_->version());

    if (!out_meta->has_communicationid()) {
        out_meta->set_communicationid(in_meta->communicationid());
    }
    out_meta->set_composition(in_meta->composition());
    // out_meta->set_executiontime(execution_time);
    out_meta->set_action(in_meta->action());
}


void ServiceEngine::parse_composition(const EngineData& input)
{
    auto& current_composition = input.composition();
    if (current_composition != prev_composition_) {
        compiler_.compile(current_composition);
        prev_composition_ = current_composition;
    }
}


std::set<std::string> ServiceEngine::get_links(const EngineData&,
                                               const EngineData&)
{
    return compiler_.outputs();
}


void ServiceEngine::send_response(EngineData& output, const xmsg::Topic& topic)
{
    auto con = connect();
    auto msg = put_engine_data(output, topic);
    publish(con, msg);
}


void ServiceEngine::send_result(EngineData& output,
                                const std::set<std::string> links)
{
    for (auto&& ss : links) {
        auto host = util::get_dpe_host(ss);
        auto port = util::get_dpe_port(ss);
        auto addr = xmsg::ProxyAddress{host, port};
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


void ServiceEngine::report(const std::string& topic_prefix, EngineData& output)
{
    auto topic = xmsg::Topic::raw(topic_prefix + ":" + name());
    auto msg = accessor_.serialize(output, topic, output_types_);
    auto con = connect(frontend().addr());
    publish(con, msg);
}


namespace util {

EngineData build_error_data(const char* msg, int severity, const std::exception& ex)
{
    EngineData data;
    data.set_data(type::STRING.mime_type(), msg);
    data.set_description(ex.what());
    data.set_status(clara::EngineStatus::ERROR, severity);
    return data;
}

} // end namespace util

} // end namespace clara
