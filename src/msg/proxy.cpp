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

#include <clara/msg/proxy.hpp>

#include "constants.hpp"
#include "zhelper.hpp"

#include <iostream>
#include <mutex>

namespace {
std::mutex mtx;

}


namespace clara::msg::sys {

Proxy::Proxy(std::unique_ptr<Context> ctx, const ProxyAddress& addr)
  : ctx_{std::move(ctx)}
  , addr_{addr}
  , is_alive_{false}
{ }


Proxy::Proxy(const ProxyAddress& addr)
  : ctx_{Context::create()},
    addr_{addr},
    is_alive_{false}
{ }


Proxy::~Proxy()
{
    if (is_alive_.load()) {
        stop();
    }
}


void Proxy::start()
{
    is_alive_ = true;
    proxy_ = std::thread{&Proxy::proxy, this};
    ctrl_ = std::thread{&Proxy::control, this};
}


void Proxy::proxy()
{
    try {
        auto in = ctx_->impl_->create_socket(zmq::socket_type::xsub);
        auto out = ctx_->impl_->create_socket(zmq::socket_type::xpub);

        detail::bind(in, addr_.pub_port());
        detail::bind(out, addr_.sub_port());

        zmq::proxy(in, out);
    } catch (const zmq::error_t& e) {
        if (e.num() != ETERM) {
            std::lock_guard<std::mutex> lock(mtx);
            std::cerr << "Proxy sockets: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << e.what() << std::endl;
    }
}


void Proxy::control()
{
    auto control = ctx_->impl_->create_socket(zmq::socket_type::sub);
    auto publisher = ctx_->impl_->create_socket(zmq::socket_type::pub);
    auto router = ctx_->impl_->create_socket(zmq::socket_type::router);

    try {
        detail::connect(control, addr_.host(), addr_.sub_port());
        detail::connect(publisher, addr_.host(), addr_.pub_port());
        detail::bind(router, addr_.pub_port() + 2);
    } catch (const zmq::error_t& e) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << "Control socket: " << e.what() << std::endl;
        return;
    }

    control.set(zmq::sockopt::subscribe, constants::ctrl_topic);
    router.set(zmq::sockopt::router_handover, 1);

    while (is_alive_) {
        try {
            auto in_msg = detail::RawMessage{control};
            if (in_msg.size() != 3) {
                std::lock_guard<std::mutex> lock(mtx);
                std::cerr << "proxy: invalid multi-part control message"
                          << std::endl;
                continue;
            }

            auto& type_msg = in_msg[1];
            auto& id_msg = in_msg[2];

            auto type = detail::to_string(type_msg);

            if (type == constants::ctrl_connect) {
                router.send(id_msg, zmq::send_flags::sndmore);
                router.send(type_msg, zmq::send_flags::none);
            } else if (type == constants::ctrl_subscribe) {
                publisher.send(id_msg, zmq::send_flags::sndmore);
                publisher.send(type_msg, zmq::send_flags::none);
            } else if (type == constants::ctrl_reply) {
                router.send(id_msg, zmq::send_flags::sndmore);
                router.send(type_msg, zmq::send_flags::none);
            }
        } catch (const zmq::error_t& ex) {
            if (ex.num() != ETERM) {
                std::lock_guard<std::mutex> lock(mtx);
                std::cerr << ex.what() << std::endl;
            }
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(mtx);
            std::cerr << e.what() << std::endl;
        }
    }
}


void Proxy::stop()
{
    is_alive_ = false;
    ctx_->impl_->close();

    proxy_.join();
    ctrl_.join();
}

} // end namespace clara::msg::sys
