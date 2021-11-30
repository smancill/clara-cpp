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

#ifndef CLARA_SERVICE_HPP
#define CLARA_SERVICE_HPP

#include <clara/engine.hpp>

#include "base.hpp"
#include "concurrent_utils.hpp"
#include "service_config.hpp"
#include "service_engine.hpp"
#include "service_loader.hpp"
#include "service_report.hpp"

#include <mutex>

namespace clara {

class Service : public Base
{
public:
    Service(const Component& self,
            const Component& frontend,
            const ServiceParameters& params);

    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;

    ~Service() override;

public:
    void start();

    void stop();

    void setup(msg::Message& msg);

    void configure(msg::Message& msg);

    void execute(msg::Message& msg);

    void callback(msg::Message& msg);

public:
    std::shared_ptr<ServiceReport> report() const;

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
