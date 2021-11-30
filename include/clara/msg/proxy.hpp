/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
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
