/*
 * Copyright (C) 2015. Jefferson Lab, xMsg framework (JLAB). All Rights Reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research, and not-for-profit purposes,
 * without fee and without a signed licensing agreement.
 *
 * Contact Vardan Gyurjyan
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include "connection_driver.h"

#include <xmsg/constants.h>
#include <xmsg/util.h>

#include <array>
#include <iostream>

namespace xmsg {
namespace detail {

ProxyDriver::ProxyDriver(Context& ctx,
                         const ProxyAddress& addr,
                         std::shared_ptr<ConnectionSetup> setup)
  : addr_{addr}
  , setup_{std::move(setup)}
  , pub_{ctx.create_socket(zmq::socket_type::pub)}
  , sub_{ctx.create_socket(zmq::socket_type::sub)}
  , control_{ctx.create_socket(zmq::socket_type::dealer)}
  , id_{detail::get_random_id()}
{
    // nop
}


void ProxyDriver::connect()
{
    SocketSetup pub_setup{pub_};
    SocketSetup sub_setup{sub_};

    setup_->pre_connection(pub_setup);
    setup_->pre_connection(sub_setup);

    detail::connect(pub_, addr_.host(), addr_.pub_port());
    detail::connect(sub_, addr_.host(), addr_.sub_port());

    const auto& topic = constants::ctrl_topic;
    const auto& request = constants::ctrl_connect;
    const auto& identity = id_;

    control_.setsockopt(ZMQ_IDENTITY, identity.data(), identity.size());
    detail::connect(control_, addr_.host(), addr_.sub_port() + 1);

    auto poller = detail::BasicPoller{control_};
    auto retry = 0;
    while (retry < 10) {
        retry++;
        try {
            pub_.send(topic.data(), topic.size(), ZMQ_SNDMORE);
            pub_.send(request.data(), request.size(), ZMQ_SNDMORE);
            pub_.send(identity.data(), identity.size(), 0);

            if (poller.poll(100)) {
                auto response = detail::RawMessage{control_};
                if (response.size() == 1) {
                    break;
                }
            }
        } catch (zmq::error_t& e) {
            // TODO handle reconnect
            std::cerr << e.what() << std::endl;
        }
    }
    if (retry >= 10) {
        throw std::runtime_error{"Could not connect to " + addr_.host()};
    }

    setup_->post_connection();
}


void ProxyDriver::send(Message& msg)
{
    const auto& t = msg.topic().str();
    const auto& m = msg.meta()->SerializeAsString();
    const auto& d = msg.data();

    pub_.send(t.data(), t.size(), ZMQ_SNDMORE);
    pub_.send(m.data(), m.size(), ZMQ_SNDMORE);
    pub_.send(d.data(), d.size(), 0);
}


RawMessage ProxyDriver::recv()
{
    return detail::RawMessage{sub_};

}


void ProxyDriver::subscribe(const Topic& topic)
{
    const auto& ctrl = constants::ctrl_topic;
    const auto& request = constants::ctrl_subscribe;
    const auto& identity = topic.str();

    sub_.setsockopt(ZMQ_SUBSCRIBE, identity.data(), identity.size());

    auto poller = detail::BasicPoller{sub_};
    auto retry = 0;
    while (retry < 10) {
        retry++;
        try {
            pub_.send(ctrl.data(), ctrl.size(), ZMQ_SNDMORE);
            pub_.send(request.data(), request.size(), ZMQ_SNDMORE);
            pub_.send(identity.data(), identity.size(), 0);

            if (poller.poll(100)) {
                auto response = detail::RawMessage{sub_};
                if (response.size() == 2) {
                    break;
                }
            }
        } catch (zmq::error_t& e) {
            // TODO handle reconnect
            std::cerr << e.what() << std::endl;
        }
    }
    if (retry >= 10) {
        throw std::runtime_error{"Could not subscribe to " + addr_.host()};
    }
}


void ProxyDriver::unsubscribe(const Topic& topic)
{
    auto& str = topic.str();
    sub_.setsockopt(ZMQ_UNSUBSCRIBE, str.data(), str.size());
}


const ProxyAddress& ProxyDriver::address()
{
    return addr_;
}


Message parse_message(RawMessage& multi_msg)
{
    auto topic = detail::to_string(multi_msg[0]);
    auto meta = proto::make_meta();
    meta->ParseFromArray(multi_msg[1].data(), multi_msg[1].size());

    auto data_ptr = multi_msg[2].data<std::uint8_t>();
    auto data_size = multi_msg[2].size();
    auto data = std::vector<std::uint8_t>(data_ptr, data_ptr + data_size);

    return {Topic::raw(topic), std::move(meta), std::move(data)};
}

} // end namespace detail
} // end namespace xmsg
