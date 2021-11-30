/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "base.hpp"

#include "constants.hpp"
#include "data_utils.hpp"

#include <clara/msg/mimetype.hpp>

#include <cstdint>
#include <iostream>
#include <vector>

namespace {
    auto get_fe_address(const clara::Component& fe) -> clara::msg::RegAddress
    {
        int reg_port = fe.addr().pub_port() + clara::constants::reg_port_shift;
        return {fe.addr().host(), reg_port};
    }
}


namespace clara {

Base::Base(const Component& self, const Component& frontend)
  : msg::Actor{self.name(), self.addr(), get_fe_address(frontend)}
  , self_{self}
  , frontend_{frontend}
{
    // nop
}

Base::~Base() = default;


void Base::send(const Component& component, const std::string& data)
{
    auto msg = util::build_request(component.topic(), data);
    auto con = connect(component.addr());
    publish(con, msg);
}


void Base::send(const Component& component, msg::Message& msg)
{
    auto con = connect(component.addr());
    publish(con, msg);
}


void Base::send_response(const msg::Message& msg,
                         const std::string& data,
                         msg::proto::Meta::Status status)
{
    auto meta = std::make_unique<msg::proto::Meta>();
    msg::proto::detail::set_datatype(*meta, msg::mimetype::single_string);
    meta->set_author(name());
    meta->set_status(status);
    auto res = msg::Message{msg.replyto(), std::move(meta),
                            std::vector<std::uint8_t>{data.begin(), data.end()}};

    auto con = connect();
    publish(con, res);
}

} // end namespace clara
