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

#ifndef CLARA_SERVICE_ENGINE_HPP
#define CLARA_SERVICE_ENGINE_HPP

#include <clara/engine.hpp>

#include "base.hpp"

#include <memory>
#include <mutex>

namespace clara {

class ServiceEngine : public Base
{
public:
    ServiceEngine(const Component& self,
                  const Component& frontend,
                  std::unique_ptr<Engine>&& engine);

    ServiceEngine(const ServiceEngine&) = delete;
    ServiceEngine& operator=(const ServiceEngine&) = delete;

    ~ServiceEngine();

public:
    void setup(xmsg::Message& msg);

    void configure(xmsg::Message& msg);

    void execute(xmsg::Message& msg);

private:
    std::mutex mutex_;

    std::unique_ptr<Engine> engine_;
};

} // end namespace clara

#endif // end of include guard: CLARA_SERVICE_ENGINE_HPP
