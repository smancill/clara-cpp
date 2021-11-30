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

#ifndef CLARA_MSG_PROXY_H_
#define CLARA_MSG_PROXY_H_

#include <clara/msg/address.hpp>
#include <clara/msg/context.hpp>

#include <atomic>
#include <memory>
#include <thread>

namespace clara::msg::sys {

class Proxy final
{
public:
    Proxy(std::unique_ptr<Context> ctx, ProxyAddress addr);
    explicit Proxy(ProxyAddress addr);

    Proxy(const Proxy&) = delete;

    auto operator=(const Proxy&) -> Proxy& = delete;

    ~Proxy();

public:
    void start();
    void stop();

private:
    void proxy();
    void control();

private:
    using ProxyContext = std::unique_ptr<Context>;

    ProxyContext ctx_;
    ProxyAddress addr_;

    std::atomic_bool is_alive_;
    std::thread proxy_;
    std::thread ctrl_;
};

} // end namespace clara::msg::sys

#endif // CLARA_MSG_PROXY_H_
