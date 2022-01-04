/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "container.hpp"

#include "logging.hpp"

#include <cstdlib>
#include <exception>

namespace {
auto default_author() -> std::string
{
    auto* author = std::getenv("USER");  // NOLINT(concurrency-mt-unsafe): setenv is never used
    return author != nullptr ? author : "clara";
}
} // end namespace


namespace clara {

Container::Container(const Component& self,
                     const Component& frontend,
                     std::string_view description)
  : Base{self, frontend}
  , report_{std::make_shared<ContainerReport>(name(), default_author())}
  , description_{description}
  , running_{false}
{
    // nop
}

Container::~Container()
{
    try {
        stop();
    } catch (...) {
        // nop
    }
}


void Container::start()
{
    std::unique_lock<std::mutex> lock{mutex_};
    register_as_subscriber(self().topic(), description_);
    running_ = true;
    LOGGER->info("started container = %s", name());
}


void Container::stop()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (running_) {
        running_ = false;
        remove_services();
        try {
            deregister_as_subscriber(self().topic());
            LOGGER->info("removed container = %s", name());
        } catch (const std::exception& e) {
            LOGGER->error("could not remove container = %s", name());
        }
    }
}


void Container::add_service(const ServiceParameters& params)
{
    auto name = params.engine_name;
    auto serv_comp = Component::service(self(), name);
    auto service = services_.insert(name, serv_comp, frontend(), params);
    if (service) {
        try {
            service->start();
            report_->add_service(service->report());
        } catch (const std::exception& e) {
            service->stop();
            services_.remove(name);
            throw;
        }
    } else {
        LOGGER->error("service = %s already exists. No new service is deployed",
                      serv_comp.name());
    }
}


auto Container::remove_service(const std::string& engine_name) -> bool
{
    auto service = services_.remove(engine_name);
    if (service) {
        service->stop();
        report_->remove_service(service->report());
        return true;
    }
    return false;
}


void Container::remove_services()
{
    services_.for_each([](auto s) { s->stop(); });
    services_.clear();
}


auto Container::report() const -> std::shared_ptr<ContainerReport>
{
    return report_;
}

} // end namespace clara
