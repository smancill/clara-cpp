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

#include "service.hpp"

#include "data_utils.hpp"
#include "logging.hpp"

namespace clara {

Service::Service(const Component& self,
                 const Component& frontend,
                 const ServiceParameters& params)
  : Base{self, frontend}
  , service_{std::make_unique<ServiceEngine>(self, frontend, nullptr)}
{
    LOGGER->info("engine loading not implemetend (service = %s)", name());
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


void Service::setup(xmsg::Message& msg)
{
    LOGGER->info("engine setup not implemetend (service = %s)", name());
}


void Service::configure(xmsg::Message& msg)
{
    service_->configure(msg);
}


void Service::execute(xmsg::Message& msg)
{
    service_->execute(msg);
}


void Service::callback(xmsg::Message& msg)
{
    std::unique_lock<std::mutex> lock{cb_mutex_};
    try {
        auto meta = msg.meta();
        if (!meta->has_action()) {
            setup(msg);
        } else if (meta->action() == xmsg::proto::Meta::CONFIGURE) {
            configure(msg);
        } else {
            execute(msg);
        }
    } catch (std::exception& e) {
        LOGGER->error(e.what());
    }
}

} // end namespace clara
