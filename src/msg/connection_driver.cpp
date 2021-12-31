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

#include "connection_driver.hpp"

#include "constants.hpp"

#include <iostream>

namespace clara::msg::detail {

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

    control_.set(zmq::sockopt::routing_id, identity);
    detail::connect(control_, addr_.host(), addr_.sub_port() + 1);

    auto poller = detail::BasicPoller{control_};
    auto retry = 0;
    while (retry < 10) {
        retry++;
        try {
            using zmq::send_flags;

            pub_.send(detail::buffer(topic), send_flags::sndmore);
            pub_.send(detail::buffer(request), send_flags::sndmore);
            pub_.send(detail::buffer(identity), send_flags::none);

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

    using zmq::send_flags;

    pub_.send(detail::buffer(t), send_flags::sndmore);
    pub_.send(detail::buffer(m), send_flags::sndmore);
    pub_.send(detail::buffer(d), send_flags::none);
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

    sub_.set(zmq::sockopt::subscribe, identity);

    auto poller = detail::BasicPoller{sub_};
    auto retry = 0;
    while (retry < 10) {
        retry++;
        try {
            using zmq::send_flags;

            pub_.send(detail::buffer(ctrl), send_flags::sndmore);
            pub_.send(detail::buffer(request), send_flags::sndmore);
            pub_.send(detail::buffer(identity), send_flags::none);

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
    sub_.set(zmq::sockopt::unsubscribe, topic.str());
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

} // end namespace clara::msg::detail
