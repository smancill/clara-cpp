/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_SUBSCRIPTION_H_
#define CLARA_MSG_SUBSCRIPTION_H_

#include <clara/msg/topic.hpp>

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace clara::msg {

namespace detail {
class ProxyDriver;
}

class Message;
class Actor;

/**
 * The handler for an active subscription.
 */
class Subscription final
{
public:
    Subscription(const Subscription&) = delete;

    auto operator=(const Subscription&) -> Subscription& = delete;

    ~Subscription();

private:
    using Callback = std::function<void(Message&)>;
    using ConnectionDeleter = std::function<void(detail::ProxyDriver*)>;
    using ConnectionWrapperPtr = std::unique_ptr<detail::ProxyDriver, ConnectionDeleter>;

    Subscription(const Topic& topic,
                 ConnectionWrapperPtr connection,
                 Callback handler);

    void run();
    void stop();

private:
    friend Actor;

    Topic topic_;
    ConnectionWrapperPtr connection_;
    std::function<void(Message&)> handler_;

    std::thread thread_;
    std::atomic_bool is_alive_;
};

} // end namespace clara::msg

#endif // CLARA_MSG_SUBSCRIPTION_H_
