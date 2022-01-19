/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_REG_DRIVER_HPP_
#define CLARA_MSG_REG_DRIVER_HPP_

#include <clara/msg/address.hpp>
#include <clara/msg/proto/registration.hpp>
#include <clara/msg/topic.hpp>

#include "zhelper.hpp"

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace clara::msg {

namespace detail {

using RequestMsg = std::array<zmq::message_t, 3>;
using ResponseMsg = std::vector<zmq::message_t>;


class Request final
{
public:
    Request(std::string_view action,
            std::string_view sender,
            const proto::Registration& data);

    explicit Request(RequestMsg&& msg);

    auto msg() -> RequestMsg& { return msg_; };

    auto action() const & -> std::string_view { return to_string_view(msg_[0]); }
    auto action() && = delete;

    auto sender() const & -> std::string_view { return to_string_view(msg_[1]); }
    auto sender() && = delete;

    auto data() const -> proto::Registration;

    friend auto operator==(const Request& lhs, const Request& rhs) -> bool;

private:
    RequestMsg msg_;
};


class Response final
{
public:
    Response(std::string_view action,
             std::string_view sender);

    Response(std::string_view action,
             std::string_view sender,
             const RegDataSet& data);

    Response(std::string_view action,
             std::string_view sender,
             std::string_view error_msg);

    explicit Response(ResponseMsg&& msg);

    auto msg() -> ResponseMsg& { return msg_; };

    auto action() const & -> std::string_view { return to_string_view(msg_[0]); }
    auto action() && = delete;

    auto sender() const & -> std::string_view { return to_string_view(msg_[1]); }
    auto sender() && = delete;

    auto status() const & -> std::string_view { return to_string_view(msg_[2]); }
    auto status() && = delete;

    auto data() const -> RegDataSet;

    friend auto operator==(const Response& lhs, const Response& rhs) -> bool;

private:
    static constexpr int n_fields = 3;

    ResponseMsg msg_;
};


class RegDriver
{
public:
    RegDriver(Context& ctx, RegAddress addr);

    RegDriver(const RegDriver&) = delete;
    RegDriver(RegDriver&&) noexcept = default;

    auto operator=(const RegDriver&) -> RegDriver& = delete;
    auto operator=(RegDriver&&) noexcept -> RegDriver& = default;

    virtual ~RegDriver() = default;

public:
    void add(std::string_view sender, const proto::Registration& data);

    void remove(std::string_view sender, const proto::Registration& data);
    void remove_all(std::string_view sender, std::string_view host);

    auto find(std::string_view sender, const proto::Registration& data) -> RegDataSet;

    auto address() const -> const RegAddress& { return addr_; }

private:
    virtual auto request(Request& req, int timeout) -> Response;

private:
    RegAddress addr_;
    zmq::socket_t socket_;
};


auto operator==(const Request& lhs, const Request& rhs) -> bool;
auto operator!=(const Request& lhs, const Request& rhs) -> bool;

auto operator==(const Response& lhs, const Response& rhs) -> bool;
auto operator!=(const Response& lhs, const Response& rhs) -> bool;

} // end namespace detail


namespace registration {

auto create(std::string_view name,
            std::string_view description,
            std::string_view host,
            int port,
            const Topic& topic,
            proto::Registration::Type type) -> proto::Registration;

auto filter(proto::Registration::Type type) -> proto::Registration;

} // end namespace registration

} // end namespace clara::msg

#endif // CLARA_MSG_REG_DRIVER_HPP_
