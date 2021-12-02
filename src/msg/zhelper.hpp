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

#ifndef CLARA_MSG_ZHELPER_H
#define CLARA_MSG_ZHELPER_H

#include <zmq.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace clara::msg::detail {

class Context
{
public:
    auto create_socket(zmq::socket_type type) -> zmq::socket_t
    {
        auto out = zmq::socket_t{ctx_, type};
        out.set(zmq::sockopt::rcvhwm, 0);
        out.set(zmq::sockopt::sndhwm, 0);
        return out;
    }

    void set_option(int opt, int val)
    {
        int rc = zmq_ctx_set(static_cast<void*>(ctx_), opt, val);
        if (rc < 0) {
            throw zmq::error_t{};
        }
    }

    auto get_option(int opt) -> int
    {
        int rc = zmq_ctx_get(static_cast<void*>(ctx_), opt);
        if (rc < 0) {
            throw zmq::error_t{};
        }
        return rc;
    }

    void close()
    {
        ctx_.close();
    }

private:
    zmq::context_t ctx_;
};


class BasicPoller final
{
public:
    explicit BasicPoller(zmq::socket_t& socket)
      : items_{zmq::pollitem_t{static_cast<void*>(socket), 0, ZMQ_POLLIN, 0}}
    { }

    auto poll(int timeout) -> bool
    {
        zmq::poll(items_.data(), 1, std::chrono::milliseconds{timeout});
        return (items_[0].revents & ZMQ_POLLIN) != 0;
    }

private:
    using Items = std::array<zmq::pollitem_t, 1>;
    Items items_;
};


class RawMessage {
public:
    RawMessage(zmq::socket_t& socket);

    auto operator[](int idx) -> auto&
    {
        return parts_[idx];
    }

    auto size() const -> int
    {
        return counter_;
    }

private:
    static constexpr int msg_size = 3;

    int counter_ = 0;
    std::array<zmq::message_t, msg_size> parts_;
};


inline
void bind(zmq::socket_t& socket, int port)
{
    socket.bind("tcp://*:" + std::to_string(port));
}


inline
void connect(zmq::socket_t& socket, const std::string& host, int port)
{
    socket.connect("tcp://" + host + ":" + std::to_string(port));
}


template<typename C>
auto buffer(const C& data) -> zmq::const_buffer
{
    return {data.data(), data.size()};
}

template<typename C, typename = std::enable_if_t<std::is_rvalue_reference_v<C&&>>>
auto buffer(C&& data) = delete;


inline
auto to_string(const zmq::message_t& msg) -> std::string
{
    return {msg.data<const char>(), msg.size()};
}

inline
auto to_string_view(const zmq::message_t& msg) -> std::string_view
{
    return {msg.data<const char>(), msg.size()};
}

auto to_string_view(zmq::message_t&& msg) = delete;


inline
auto to_bytes(const zmq::message_t& msg) -> std::vector<std::uint8_t>
{
    const auto* ptr = msg.data<std::uint8_t>();
    return {ptr, ptr + msg.size()};
}


auto get_unique_replyto(const std::string& subject) -> std::string;

void set_unique_replyto(std::uint_fast32_t value);

auto encode_identity(const std::string& address, const std::string& name)
    -> std::string;

auto get_random_id() -> std::string;

} // end namespace clara::msg::detail

#endif // CLARA_MSG_ZHELPER_H
