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

#ifndef CLARA_SERVICE_ENGINE_HPP
#define CLARA_SERVICE_ENGINE_HPP

#include <clara/engine.hpp>

#include "base.hpp"
#include "composition.hpp"
#include "engine_data_helper.hpp"

#include <memory>
#include <mutex>

namespace clara {

class ServiceConfig;
class ServiceReport;

class ServiceEngine : public Base
{
public:
    ServiceEngine(const Component& self,
                  const Component& frontend,
                  Engine* engine,
                  ServiceReport* report,
                  ServiceConfig* config);

    ServiceEngine(const ServiceEngine&) = delete;
    ServiceEngine& operator=(const ServiceEngine&) = delete;

    ~ServiceEngine() override;

public:
    void setup(msg::Message& msg);

    void configure(msg::Message& msg);

    void execute(msg::Message& msg);

private:
    EngineData configure_engine(EngineData& input);

    EngineData execute_engine(EngineData& input);

private:
    EngineData get_engine_data(msg::Message& msg);

    msg::Message put_engine_data(const EngineData& output, const msg::Topic& topic);
    msg::Message put_engine_data(const EngineData& output, const std::string& receiver);

    void update_metadata(const EngineData& input, EngineData& output);

private:
    void parse_composition(const EngineData& input);

    std::set<std::string> get_links(const EngineData& input, const EngineData& output);

private:
    void send_response(EngineData& output, const msg::Topic& topic);
    void send_result(EngineData& output, const std::set<std::string>& links);

    void report_problem(EngineData& output);
    void report_result(EngineData& output);

    void report(const std::string& topic_prefix, EngineData& output);

private:
    std::mutex mutex_;

    Engine* engine_;
    ServiceReport* report_;
    ServiceConfig* config_;
    EngineDataAccessor accessor_;

    decltype(engine_->input_data_types()) input_types_;
    decltype(engine_->output_data_types()) output_types_;

    composition::SimpleCompiler compiler_;
    std::string prev_composition_;
};

} // end namespace clara

#endif // end of include guard: CLARA_SERVICE_ENGINE_HPP
