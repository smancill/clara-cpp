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

#ifndef CLARA_COMPONENT_HPP
#define CLARA_COMPONENT_HPP

#include "constants.hpp"

#include <clara/msg/address.hpp>
#include <clara/msg/topic.hpp>

#include <string>

namespace clara {

namespace util {

std::string make_name(const std::string& host, int port, const std::string& lang);

std::string make_name(const std::string& dpe,
                      const std::string& container);

std::string make_name(const std::string& dpe,
                      const std::string& container,
                      const std::string& engine);

} // end namespace util

class Component
{
public:
    template<typename A>
    static Component dpe(A&& addr,
                         const std::string& lang = constants::cpp_lang)
    {
        auto cname = util::make_name(addr.host(), addr.pub_port(), lang);
        auto topic = msg::Topic::build("dpe", cname);
        return Component{std::move(cname), std::forward<A>(addr), std::move(topic)};
    }

    static Component container(const Component& dpe, const std::string& name)
    {
        auto cname = util::make_name(dpe.name(), name);
        auto topic = msg::Topic::build("container", cname);
        return Component{std::move(cname), dpe.addr_, std::move(topic)};
    }

    static Component service(const Component& container,
                             const std::string& name)
    {
        auto cname = util::make_name(container.name(), name);
        auto topic = msg::Topic::raw(cname);
        return Component{std::move(cname), container.addr_, std::move(topic)};
    }

private:
    template<typename S, typename A, typename T>
    Component(S&& name, A&& addr, T&& topic)
      : name_{std::forward<S>(name)}
      , addr_{std::forward<A>(addr)}
      , topic_{std::forward<T>(topic)}
    { }

public:
    const std::string& name() const { return name_; }

    const msg::ProxyAddress& addr() const { return addr_; }

    const msg::Topic& topic() const { return topic_; }

private:
    std::string name_;
    msg::ProxyAddress addr_;
    msg::Topic topic_;
};

} // end namespace clara

#endif // end of include guard: CLARA_COMPONENT_HPP
