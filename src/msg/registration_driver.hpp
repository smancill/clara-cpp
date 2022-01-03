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

#ifndef CLARA_MSG_REG_DRIVER_HPP_
#define CLARA_MSG_REG_DRIVER_HPP_

#include <clara/msg/address.hpp>
#include <clara/msg/proto/registration.hpp>
#include <clara/msg/topic.hpp>

#include "zhelper.hpp"

#include <array>
#include <string>
#include <vector>

namespace clara::msg {

namespace detail {

using RequestMsg = std::array<zmq::message_t, 3>;
using ResponseMsg = std::vector<zmq::message_t>;


class Request final
{
public:
    Request(const std::string& topic,
            const std::string& sender,
            const proto::Registration& data);

    Request(const std::string& topic,
            const std::string& sender,
            const std::string& text);

    explicit Request(RequestMsg&& msg);

    RequestMsg& msg() { return msg_; };

    std::string topic() const { return to_string(msg_[0]); };

    std::string sender() const { return to_string(msg_[1]); };

    std::string text() const { return to_string(msg_[2]); }

    proto::Registration data() const;

    friend bool operator==(const Request& lhs, const Request& rhs);

private:
    RequestMsg msg_;
};


class Response final
{
public:
    Response(const std::string& topic,
             const std::string& sender);

    Response(const std::string& topic,
             const std::string& sender,
             const RegDataSet& data);

    Response(const std::string& topic,
             const std::string& sender,
             const std::string& error_msg);

    explicit Response(ResponseMsg&& msg);

    ResponseMsg& msg() { return msg_; };

    std::string topic() const { return to_string(msg_[0]); };

    std::string sender() const { return to_string(msg_[1]); };

    std::string status() const { return to_string(msg_[2]); }

    RegDataSet data() const;

    friend bool operator==(const Response& lhs, const Response& rhs);

private:
    static constexpr int n_fields = 3;

    ResponseMsg msg_;
};


class RegDriver
{
public:
    RegDriver(Context& ctx, RegAddress addr);

    RegDriver(const RegDriver&) = delete;
    RegDriver& operator=(const RegDriver&) = delete;

    RegDriver(RegDriver&&) noexcept = default;
    RegDriver& operator=(RegDriver&&) noexcept = default;

    virtual ~RegDriver() = default;

public:
    void add(const proto::Registration& data, bool is_publisher);

    void remove(const proto::Registration& data, bool is_publisher);
    void remove_all(const std::string& sender, const std::string& host);

    RegDataSet find(const proto::Registration& data, bool is_publisher);

    const RegAddress& address() const { return addr_; }

private:
    virtual Response request(Request& req, int timeout);

private:
    RegAddress addr_;
    zmq::socket_t socket_;
};


bool operator==(const Request& lhs, const Request& rhs);
bool operator!=(const Request& lhs, const Request& rhs);

bool operator==(const Response& lhs, const Response& rhs);
bool operator!=(const Response& lhs, const Response& rhs);

} // end namespace detail


namespace registration {

proto::Registration create(const std::string& name,
                           const std::string& description,
                           const std::string& host,
                           int port,
                           const Topic& topic,
                           bool is_publisher);

} // end namespace registration

} // end namespace clara::msg

#endif // CLARA_MSG_REG_DRIVER_HPP_
