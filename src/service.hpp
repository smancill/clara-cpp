/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_SERVICE_HPP
#define CLARA_SERVICE_HPP

#include <clara/engine.hpp>

#include "base.hpp"
#include "concurrent_utils.hpp"
#include "service_config.hpp"
#include "service_engine.hpp"
#include "service_loader.hpp"
#include "service_report.hpp"

#include <memory>
#include <mutex>

namespace clara {

class Service : public Base
{
public:
    Service(const Component& self,
            const Component& frontend,
            const ServiceParameters& params);

    Service(const Service&) = delete;

    auto operator=(const Service&) -> Service& = delete;

    ~Service() override;

public:
    void start();

    void stop();

    void setup(msg::Message& msg);

    void configure(msg::Message& msg);

    void execute(msg::Message& msg);

    void callback(msg::Message& msg);

public:
    auto report() const -> std::shared_ptr<ServiceReport>;

private:
    std::mutex mutex_;
    std::mutex cb_mutex_;

    ServiceLoader loader_;
    util::ThreadPool thread_pool_;

    std::shared_ptr<ServiceConfig> sys_config_;
    std::shared_ptr<ServiceReport> report_;
    std::unique_ptr<ServiceEngine> service_;

    std::unique_ptr<msg::Subscription> sub_;
};

} // end namespace clara

#endif // end of include guard: CLARA_SERVICE_HPP
