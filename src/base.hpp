/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_BASE_HPP
#define CLARA_BASE_HPP

#include "component.hpp"

#include <clara/msg/actor.hpp>

namespace clara {

class Base : public msg::Actor
{
public:
    Base(const Component& self, const Component& frontend);

    ~Base() override;

public:
    void send(const Component& component, const std::string& data);

    void send(const Component& component, msg::Message& msg);

    void send_response(const msg::Message& msg,
                       const std::string& data,
                       msg::proto::Meta::Status status);

public:
    auto self() -> const Component& { return self_; }

    auto frontend() -> const Component& { return frontend_; }

private:
    Component self_;
    Component frontend_;
};

} // end namespace clara

#endif // end of include guard: CLARA_BASE_HPP
