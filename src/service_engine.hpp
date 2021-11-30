/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_SERVICE_ENGINE_HPP
#define CLARA_SERVICE_ENGINE_HPP

#include <clara/engine.hpp>

#include "base.hpp"
#include "composition.hpp"
#include "engine_data_helper.hpp"

#include <mutex>
#include <set>
#include <string>

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

    auto operator=(const ServiceEngine&) -> ServiceEngine& = delete;

    ~ServiceEngine() override;

public:
    void setup(msg::Message& msg);

    void configure(msg::Message& msg);

    void execute(msg::Message& msg);

private:
    auto configure_engine(EngineData& input) -> EngineData;

    auto execute_engine(EngineData& input) -> EngineData;

private:
    auto get_engine_data(msg::Message& msg) -> EngineData;

    auto put_engine_data(const EngineData& output,
                         const msg::Topic& topic) -> msg::Message;

    auto put_engine_data(const EngineData& output,
                         const std::string& receiver) -> msg::Message;

    void update_metadata(const EngineData& input, EngineData& output);

private:
    void parse_composition(const EngineData& input);

    auto get_links(const EngineData& input,
                   const EngineData& output) -> std::set<std::string>;

private:
    void send_response(EngineData& output, const msg::Topic& topic);
    void send_result(EngineData& output, const std::set<std::string>& links);

    void report_problem(EngineData& output);
    void report_result(EngineData& output);

    void report(std::string_view topic_prefix, EngineData& output);

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
