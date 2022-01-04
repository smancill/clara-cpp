/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_CONNECTION_SETUP_H_
#define CLARA_MSG_CONNECTION_SETUP_H_

#include <cstddef>
#include <type_traits>

namespace clara::msg {

/**
 * Sets options on a new ZMQ socket.
 *
 * \see <a href="http://api.zeromq.org/4-1:zmq-setsockopt">zmq_setsockopt</a>
 */
class SocketSetup
{
public:
    explicit SocketSetup(void* socket)
      : socket_{socket}
    { }

    SocketSetup(const SocketSetup& rhs) = delete;
    auto operator=(const SocketSetup& rhs) -> SocketSetup& = delete;

    SocketSetup(SocketSetup&&) noexcept = default;
    auto operator=(SocketSetup&&) noexcept -> SocketSetup& = default;

    ~SocketSetup() = default;

public:
    /// Sets the value of a ØMQ socket option
    void set_option(int opt, const void* val, std::size_t val_len);

    /// Sets the value of a ØMQ socket option
    template <typename Integer,
              typename = std::enable_if_t<std::is_integral<Integer>::value>>
    void set_option(int opt, const Integer& val)
    {
        set_option(opt, &val, sizeof(Integer));
    }

    /// Gets the value of a ØMQ socket option
    void get_option(int opt, void* val, std::size_t* val_len) const;

    /// Gets the value of a ØMQ socket option
    template <typename Integer,
              typename = std::enable_if_t<std::is_integral<Integer>::value>>
    auto get_option(int opt) const -> Integer
    {
        Integer val;
        auto val_len = sizeof(Integer);
        get_option(opt, &val, &val_len);
        return val;
    }

    /// Gets the type of the 0MQ socket
    auto type() const -> int;

private:
    void* socket_;
};


/**
 * Advanced setup of a connection to a Clara proxy.
 *
 * This class can be used to customize the internal sockets of a new Connection
 * created by the Clara actor.
 */
class ConnectionSetup
{
public:
    ConnectionSetup() = default;

    ConnectionSetup(const ConnectionSetup&) = delete;

    auto operator=(const ConnectionSetup&) -> ConnectionSetup& = delete;

    virtual ~ConnectionSetup() = default;

    /**
     * Configures the socket before it is connected.
     * This method will be called for both pub/sub sockets.
     * It should be used to set options on the socket.
     *
     * Leave empty if no configuration is required.
     */
    virtual void pre_connection(SocketSetup& /*socket*/) {};

    /**
     * Runs after the two sockets have been connected.
     * This method can be used to run some action after connecting the sockets.
     * For example, sleep a while to give time to the sockets to be actually
     * connected internally.
     *
     * Leave empty if no action is required.
     */
    virtual void post_connection() {};
};

} // end namespace clara::msg

#endif // CLARA_MSG_CONNECTION_SETUP_H_
