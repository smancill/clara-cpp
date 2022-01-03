/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/subscription.hpp>

#include "connection_driver.hpp"
#include "likely.hpp"

#include <exception>
#include <iostream>

namespace clara::msg {

/**
 * \class Subscription
 *
 * A subscription object uses a \ref ProxyConnection "connection" to receive
 * \ref Message "messages" of the interested \ref Topic "topic",
 * and calls a user action on every message.
 *
 * When the subscription is constructed, the connection will be subscribed to
 * the topic, and a background thread will be started polling the connection for
 * received messages. For every message, the user-provide callback will be
 * executed.
 *
 * When the subscription is destroyed, the background thread will be stopped
 * and the connection will be unsubscribed from the topic.
 *
 * Creation and destruction of subscriptions are controlled by the pub-sub
 * actor.
 */

Subscription::Subscription(const Topic& topic,  // NOLINT(modernize-pass-by-value)
                           ConnectionWrapperPtr connection,
                           Callback handler)
  : topic_{topic}
  , connection_{std::move(connection)}
  , handler_{std::move(handler)}
  , is_alive_{false}
{
    connection_->subscribe(topic_);
    is_alive_.store(true);
    thread_ = std::thread{&Subscription::run, this};
}


Subscription::~Subscription()
{
    if (is_alive_) {
        stop();
    }
}


void Subscription::run()
{
    auto poller = detail::BasicPoller{connection_->sub_socket()};
    const int timeout = 100;
    while (is_alive_.load()) {
        try {
            if (poller.poll(timeout)) {
                auto raw_msg = connection_->recv();
                if (CLARA_LIKELY(raw_msg.size() == 3)) {
                    auto msg = detail::parse_message(raw_msg);
                    handler_(msg);
                }
            }
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}


void Subscription::stop()
{
    is_alive_ = false;
    thread_.join();
    connection_->unsubscribe(topic_);
}

} // end namespace clara::msg
