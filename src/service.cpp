/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "service.hpp"

#include "logging.hpp"


static constexpr auto default_queue_size = 1024;


static auto thread_pool_options(int pool_size, int queue_size)
    -> tp::ThreadPoolOptions
{
    auto opts = tp::ThreadPoolOptions{};
    opts.setThreadCount(pool_size);
    opts.setQueueSize(queue_size);
    return opts;
}


namespace clara {

Service::Service(const Component& self,
                 const Component& frontend,
                 const ServiceParameters& params)
  : Base{self, frontend}
  , loader_{params.engine_lib}
  , thread_pool_{thread_pool_options(params.pool_size, default_queue_size)}
  , sys_config_{std::make_shared<ServiceConfig>()}
  , report_{std::make_shared<ServiceReport>(name(), params,
                                            loader_->author(),
                                            loader_->version(),
                                            loader_->description())}
  , service_{std::make_unique<ServiceEngine>(self, frontend,
                                             loader_.get(),
                                             report_.get(),
                                             sys_config_.get())}
{
    LOGGER->info("created service = %s pool_size = %d", name(), params.pool_size);
}


Service::~Service()
{
    try {
        stop();
    } catch (...) {
        // nop
    }
}


void Service::start()
{
    std::unique_lock<std::mutex> lock{mutex_};
    sub_ = subscribe(self().topic(), connect(), [this](auto& msg) {
        this->callback(msg);
    });
    register_as_subscriber(self().topic(), "");
    LOGGER->info("started service = %s", name());
}


void Service::stop()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (sub_) {
        try {
            unsubscribe(std::move(sub_));
            deregister_as_subscriber(self().topic());
            LOGGER->info("removed service = %s", name());
        } catch (const std::exception& e) {
            LOGGER->error("could not remove service = %s", name());
        }
    }
}


void Service::setup(msg::Message& msg)
{
    auto m = std::make_unique<msg::Message>(std::move(msg));
    thread_pool_.post([s=service_.get(), m=std::move(m)]() { s->setup(*m); });
}


void Service::configure(msg::Message& msg)
{
    auto m = std::make_unique<msg::Message>(std::move(msg));
    thread_pool_.post([s=service_.get(), m=std::move(m)](){
        try {
            s->configure(*m);
        } catch (const std::exception& e) {
            LOGGER->error("%s configure: unhandled exception %s", s->name(), e.what());
        } catch (...) {
            LOGGER->error("%s configure: unexpected exception", s->name());
        }
    });
}


void Service::execute(msg::Message& msg)
{
    auto m = std::make_unique<msg::Message>(std::move(msg));
    thread_pool_.post([s=service_.get(), m=std::move(m)]() {
        try {
            s->execute(*m);
        } catch (const std::exception& e) {
            LOGGER->error("%s execute: unhandled exception %s", s->name(), e.what());
        } catch (...) {
            LOGGER->error("%s execute: unexpected exception", s->name());
        }
    });
}


void Service::callback(msg::Message& msg)
{
    std::unique_lock<std::mutex> lock{cb_mutex_};
    try {
        const auto* meta = msg.meta();
        if (!meta->has_action()) {
            setup(msg);
        } else if (meta->action() == msg::proto::Meta::CONFIGURE) {
            configure(msg);
        } else {
            execute(msg);
        }
    } catch (std::exception& e) {
        LOGGER->error("%s callback: %s", name(), e.what());
    } catch (...) {
        LOGGER->error("%s callback: unexpected exception", name());
    }
}


auto Service::report() const -> std::shared_ptr<ServiceReport>
{
    return report_;
}

} // end namespace clara
