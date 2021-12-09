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

#include "base.hpp"

#include "constants.hpp"
#include "data_utils.hpp"

#include <clara/msg/mimetype.hpp>

#include <algorithm>
#include <functional>
#include <iostream>

namespace {
    clara::msg::RegAddress get_fe_address(const clara::Component& fe)
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


Base::~Base()
{
    // nothing
}


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
    meta->set_datatype(msg::mimetype::single_string);
    meta->set_author(name());
    meta->set_status(status);
    auto res = msg::Message{msg.replyto(), std::move(meta),
                            std::vector<std::uint8_t>{data.begin(), data.end()}};

    auto con = connect();
    publish(con, res);
}

} // end namespace clara
