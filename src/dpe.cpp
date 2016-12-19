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

#include "dpe.hpp"

#include "base.hpp"
#include "concurrent_map.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "data_utils.hpp"
#include "logging.hpp"
#include "utils.hpp"

#include <xmsg/proxy.h>
#include <xmsg/xmsg.h>

#include <mutex>
#include <thread>
#include <unordered_map>

namespace clara {

class DpeException : public std::runtime_error
{
public:
    DpeException(std::string msg) : std::runtime_error{std::move(msg)} { }
};


class Dpe::DpeImpl : public Base
{
public:
    DpeImpl(const xmsg::ProxyAddress& local,
            const xmsg::ProxyAddress& frontend,
            const DpeConfig& config)
      : Base{Component::dpe(local),
             Component::dpe(frontend, constants::java_lang)}
      , proxy_{std::make_unique<xmsg::sys::Proxy>(local)}
      , config_{config}
    {
        // nop
    }

    ~DpeImpl() override = default;

public:
    void start_proxy();

    void stop_proxy();

    void print_startup();

    void subscribe();

    void unsubscribe();

    void start_container(util::RequestParser& parser);

    void stop_container(util::RequestParser& parser);

    void stop_containers();

    void start_service(util::RequestParser& parser);

    void stop_service(util::RequestParser& parser);

    void callback(xmsg::Message& msg);

public:
    xmsg::Topic topic()
    {
        return xmsg::Topic::build("dpe", name());
    }

private:
    std::mutex dpe_mutex_;
    std::mutex cb_mutex_;

    std::unique_ptr<xmsg::sys::Proxy> proxy_;
    std::unique_ptr<xmsg::Subscription> sub_;
    util::ConcurrentMap<std::string, Container> containers_;

    DpeConfig config_;
};


Dpe::Dpe(bool is_frontend,
         const xmsg::ProxyAddress& local,
         const xmsg::ProxyAddress& frontend,
         const DpeConfig& config)
  : dpe_{std::make_unique<DpeImpl>(local, frontend, config)}
{
    // nop
}


Dpe::~Dpe()
{
    stop();
}


void Dpe::start()
{
    dpe_->start_proxy();
    dpe_->subscribe();
    dpe_->print_startup();
}


void Dpe::stop()
{
    dpe_->unsubscribe();
    dpe_->stop_containers();
    dpe_->stop_proxy();
    LOGGER->info("shutdown DPE");
}


void Dpe::DpeImpl::start_proxy()
{
    proxy_->start();
}


void Dpe::DpeImpl::stop_proxy()
{
    proxy_->stop();
}


void Dpe::DpeImpl::stop_containers()
{
    containers_.for_each([](auto s) { s->stop(); });
    containers_.clear();
}


void Dpe::DpeImpl::print_startup()
{
    std::cout << "=========================================" << std::endl;
    std::cout << "                 CLARA DPE               " << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << " Name             = " << name() << std::endl;
    std::cout << " Date             = " << util::get_current_time() << std::endl;
    std::cout << " Version          = 4.3" << std::endl;
    std::cout << " Lang             = " << "C++" << std::endl;
    if (!config_.description.empty()) {
        std::cout << " Description      = " << config_.description << std::endl;
    }
    std::cout << std::endl;
    std::cout << " Proxy Host       = " << self().addr().host() << std::endl;
    std::cout << " Proxy Port       = " << self().addr().pub_port() << std::endl;
    std::cout << std::endl;
    std::cout << " FrontEnd Host    = " << frontend().addr().host() << std::endl;
    std::cout << " FrontEnd Port    = " << frontend().addr().pub_port() << std::endl;
    std::cout << " FrontEnd Lang    = " << "Java" << std::endl;
    std::cout << "=========================================" << std::endl;
}


void Dpe::DpeImpl::subscribe()
{
    std::unique_lock<std::mutex> lock{dpe_mutex_};
    sub_ = Base::subscribe(topic(), connect(), [this](auto& msg) {
        this->callback(msg);
    });
    Base::register_as_subscriber(topic(), config_.description);
}


void Dpe::DpeImpl::unsubscribe()
{
    std::unique_lock<std::mutex> lock{dpe_mutex_};
    if (sub_) {
        Base::unsubscribe(std::move(sub_));
        Base::deregister_as_subscriber(topic());
    }
}


void Dpe::DpeImpl::start_container(util::RequestParser& parser)
{
    auto name = parser.next_string();
    auto pool_size = parser.next_integer();
    auto description = parser.next_string();
    if (pool_size <= 0) {
        pool_size = 1;
    }

    auto cont_comp = Component::container(self(), name);
    auto container = containers_.insert(name, cont_comp, frontend(), "");
    if (container) {
        try {
            container->start();
        } catch (const std::exception& e) {
            container->stop();
            containers_.remove(name);
            throw DpeException{"could not start container " + cont_comp.name() +
                               ": " + e.what()};
        }
    } else {
        throw util::InvalidRequest{
                "container = " + name + " already exists. " +
                "No new container is created."};
    }
}


void Dpe::DpeImpl::start_service(util::RequestParser& parser)
{
    auto container_name = parser.next_string();
    auto engine_name = parser.next_string();
    auto engine_lib = parser.next_string();
    auto pool_size = parser.next_integer();
    auto description = parser.next_string();
    auto initial_state = parser.next_string();
    if (pool_size <= 0) {
        pool_size = 1;
    }

    ServiceParameters service_params = {
        engine_name, engine_lib, initial_state, description, pool_size
    };

    auto service_name = util::make_name(name(), container_name, engine_name);
    auto container = containers_.find(container_name);
    if (container) {
        try {
            container->add_service(service_params);
        } catch (const std::exception& e) {
            throw DpeException{"could not start service " + service_name +
                               ": " + e.what()};
        }
    } else {
        throw util::InvalidRequest{
                "could not start service = " + service_name +
                ": missing container"};
    }
}


void Dpe::DpeImpl::stop_service(util::RequestParser& parser)
{
    auto container_name = parser.next_string();
    auto engine_name = parser.next_string();
    auto service_name = util::make_name(name(), container_name, engine_name);

    auto container = containers_.find(container_name);
    if (!container) {
        throw util::InvalidRequest{
                "could not stop service = " + service_name +
                ": missing container"};
    }
    bool removed = container->remove_service(engine_name);
    if (!removed) {
        throw util::InvalidRequest{
                "could not stop service = " + service_name +
                ": service doesn't exist"};
    }
}


void Dpe::DpeImpl::stop_container(util::RequestParser& parser)
{
    auto name = parser.next_string();
    auto container = containers_.remove(name);
    if (!container) {
        auto container_name = xmsg::Topic::build(DpeImpl::name(), name).str();
        throw util::InvalidRequest{"could not stop container = " + container_name +
                                   ": container doesn't exist"};
    }
    container->stop();
}


void Dpe::DpeImpl::callback(xmsg::Message& msg)
{
    std::unique_lock<std::mutex> lock{cb_mutex_};
    try {
        auto parser = util::RequestParser::build(msg);
        auto cmd = parser.next_string();

        if (cmd == constants::start_container) {
            start_container(parser);
        } else if (cmd == constants::stop_container) {
            stop_container(parser);
        } else if (cmd == constants::start_service) {
            start_service(parser);
        } else if (cmd == constants::stop_service) {
            stop_service(parser);
        } else {
            LOGGER->info("Received data %s", util::parse_message(msg));
        }
    } catch (std::exception& e) {
        LOGGER->error(e.what());
    } catch (...) {
        LOGGER->error("%s callback: unexpected exception", name());
    }
}

} // end namespace clara
