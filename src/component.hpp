/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_COMPONENT_HPP
#define CLARA_COMPONENT_HPP

#include "constants.hpp"

#include <clara/msg/address.hpp>
#include <clara/msg/topic.hpp>

#include <string>

namespace clara {

namespace util {

auto make_name(std::string_view host,
               int port,
               std::string_view lang) -> std::string;

auto make_name(std::string_view dpe,
               std::string_view container) -> std::string;

auto make_name(std::string_view dpe,
               std::string_view container,
               std::string_view engine) -> std::string;

} // end namespace util

class Component
{
public:
    template<typename A>
    static auto dpe(A&& addr,
                    std::string_view lang = constants::cpp_lang) -> Component
    {
        auto cname = util::make_name(addr.host(), addr.pub_port(), lang);
        auto topic = msg::Topic::build("dpe", cname);
        return Component{std::move(cname), std::forward<A>(addr), std::move(topic)};
    }

    static auto container(const Component& dpe,
                          std::string_view name) -> Component
    {
        auto cname = util::make_name(dpe.name(), name);
        auto topic = msg::Topic::build("container", cname);
        return Component{std::move(cname), dpe.addr_, std::move(topic)};
    }

    static auto service(const Component& container,
                        std::string_view name) -> Component
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
    auto name() const -> const std::string& { return name_; }

    auto addr() const -> const msg::ProxyAddress& { return addr_; }

    auto topic() const -> const msg::Topic& { return topic_; }

private:
    std::string name_;
    msg::ProxyAddress addr_;
    msg::Topic topic_;
};

} // end namespace clara

#endif // end of include guard: CLARA_COMPONENT_HPP
