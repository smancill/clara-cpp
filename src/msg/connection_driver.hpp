/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_CONNECTION_DRIVER_H_
#define CLARA_MSG_CONNECTION_DRIVER_H_

#include <clara/msg/address.hpp>
#include <clara/msg/connection_setup.hpp>
#include <clara/msg/message.hpp>

#include "zhelper.hpp"

#include <memory>

namespace clara::msg::detail {

/**
 * The standard pub/sub connection to a proxy.
 * Contains ProxyAddress object and two 0MQ sockets for publishing and
 * subscribing messages respectfully.
 */
class ProxyDriver final
{
public:
    ProxyDriver(Context& ctx,
                const ProxyAddress& addr,
                std::shared_ptr<ConnectionSetup> setup);

    ProxyDriver(const ProxyDriver&) = delete;
    ProxyDriver(ProxyDriver&&) = default;

    auto operator=(const ProxyDriver&) -> ProxyDriver& = delete;
    auto operator=(ProxyDriver&&) -> ProxyDriver& = default;

    ~ProxyDriver() = default;

public:
    /// Returns the address of the connected proxy
    auto address() -> const ProxyAddress&;

    /// Connects the internal sockets to the proxy
    void connect();

    /// Sends the message through the proxy
    void send(Message& msg);
    /// Receives a message through the proxy
    auto recv() -> RawMessage;

    /// Subscribes to messages of the given topic through the proxy
    void subscribe(const Topic& topic);
    /// Unsubscribes to messages of the given topic through the proxy
    void unsubscribe(const Topic& topic);

public:
    auto pub_socket() -> zmq::socket_t& { return pub_; }

    auto sub_socket() -> zmq::socket_t& { return sub_; }

private:
    ProxyAddress addr_;
    std::shared_ptr<ConnectionSetup> setup_;
    zmq::socket_t pub_;
    zmq::socket_t sub_;
    zmq::socket_t control_;
    std::string id_;
};


auto parse_message(RawMessage& msg) -> Message;

} // end namespace clara::msg::detail

#endif // CLARA_MSG_CONNECTION_DRIVER_H_
