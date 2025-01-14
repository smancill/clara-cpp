/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "registration_driver.hpp"

#include "constants.hpp"
#include "zhelper.hpp"

#include <iostream>
#include <registration.pb.h>
#include <stdexcept>
#include <string_view>
#include <tuple>

namespace {

auto make_tie(const clara::msg::proto::Registration& reg, int& port, int& owner)
{
    port = reg.port();
    owner = int{reg.type()};
    return std::tie(reg.name(), reg.topic(),
                    reg.host(), port, owner, reg.description());
}

}


namespace clara::msg {

namespace proto {

auto CompareRegistration::operator()(const Registration& lhs,
                                     const Registration& rhs) const -> bool
{
    int l_port, l_owner, r_port, r_owner;  // NOLINT
    return make_tie(lhs, l_port, l_owner) < make_tie(rhs, r_port, r_owner);
}


auto operator==(const Registration& lhs, const Registration& rhs) -> bool
{
    int l_port, l_owner, r_port, r_owner;  // NOLINT
    return make_tie(lhs, l_port, l_owner) == make_tie(rhs, r_port, r_owner);
}


auto operator!=(const Registration& lhs, const Registration& rhs) -> bool
{
    return !(lhs == rhs);
}

} // end namespace proto



namespace detail {

Request::Request(std::string_view action,
                 std::string_view sender,
                 const proto::Registration& data)
  : msg_{zmq::message_t{action},
         zmq::message_t{sender},
         zmq::message_t{data.SerializeAsString()}}
{ }


Request::Request(RequestMsg&& msg)
  : msg_{std::move(msg)}
{ }


auto Request::data() const -> proto::Registration
{
    auto rd = proto::Registration{};
    rd.ParseFromString(detail::to_string(msg_[2]));
    return rd;
}



Response::Response(std::string_view action,
                   std::string_view sender)
{
    msg_.reserve(n_fields);
    msg_.emplace_back(action);
    msg_.emplace_back(sender);
    msg_.emplace_back(constants::success);
}


Response::Response(std::string_view action,
                   std::string_view sender,
                   const RegDataSet& data)
{
    msg_.reserve(n_fields + data.size());
    msg_.emplace_back(action);
    msg_.emplace_back(sender);
    msg_.emplace_back(constants::success);
    for (const auto& reg : data) {
        msg_.emplace_back(reg.SerializeAsString());
    }
}


Response::Response(std::string_view action,
                   std::string_view sender,
                   std::string_view error_msg)
{
    msg_.reserve(n_fields);
    msg_.emplace_back(action);
    msg_.emplace_back(sender);
    msg_.emplace_back(error_msg);
}


Response::Response(ResponseMsg&& msg)
  : msg_{std::move(msg)}
{ }


auto Response::data() const -> RegDataSet
{
    auto data = RegDataSet{};
    std::for_each(msg_.begin() + n_fields, msg_.end(), [&](const zmq::message_t& f) {
        auto reg = proto::Registration{};
        reg.ParseFromArray(f.data(), static_cast<int>(f.size()));
        data.insert(reg);
    });
    return data;
}


RegDriver::RegDriver(Context& ctx, RegAddress addr)
  : addr_{std::move(addr)}
  , socket_{ctx.create_socket(zmq::socket_type::req)}
{
    socket_.set(zmq::sockopt::rcvhwm, 0);
    socket_.set(zmq::sockopt::sndhwm, 0);
    detail::connect(socket_, addr_.host(), addr_.port());
}


void RegDriver::add(std::string_view sender, const proto::Registration& data)
{
    auto req = Request{constants::reg_add, sender, data};
    request(req, constants::reg_add_timeout);
}


void RegDriver::remove(std::string_view sender, const proto::Registration& data)
{
    auto req = Request{constants::reg_remove, sender, data};
    request(req, constants::reg_remove_timeout);
}


void RegDriver::remove_all(std::string_view sender, std::string_view host)
{
    auto make_request = [&] (auto type) -> Request {
        auto data = registration::filter(type);
        data.set_host(host.data(), host.size());
        return Request{constants::reg_remove_all, sender, data};
    };

    auto pub_req = make_request(proto::Registration::PUBLISHER);
    auto sub_req = make_request(proto::Registration::SUBSCRIBER);

    request(pub_req, constants::reg_remove_timeout);
    request(sub_req, constants::reg_remove_timeout);
}


auto RegDriver::find(std::string_view sender, const proto::Registration& data)
    -> RegDataSet
{
    auto req = Request{constants::reg_find_matching, sender, data};
    auto res = request(req, constants::reg_find_timeout);
    return res.data();
}


auto RegDriver::request(Request& req, int timeout) -> Response
{
    auto& out_msg = req.msg();
    socket_.send(out_msg[0], zmq::send_flags::sndmore);
    socket_.send(out_msg[1], zmq::send_flags::sndmore);
    socket_.send(out_msg[2], zmq::send_flags::none);

    auto poller = detail::BasicPoller{socket_};
    if (poller.poll(timeout)) {
        auto in_msg = ResponseMsg{};
        while (true) {
            in_msg.emplace_back();
            auto& msg = in_msg.back();
            std::ignore = socket_.recv(msg);
            if (!msg.more()) {
                break;
            }
        }
        return Response{std::move(in_msg)};
    }
    throw std::runtime_error{"timeout"};
}



auto operator==(const Request& lhs, const Request& rhs) -> bool
{
    return lhs.msg_ == rhs.msg_;
}


auto operator!=(const Request& lhs, const Request& rhs) -> bool
{
    return !(lhs == rhs);
}


auto operator==(const Response& lhs, const Response& rhs) -> bool
{
    return lhs.msg_ == rhs.msg_;
}


auto operator!=(const Response& lhs, const Response& rhs) -> bool
{
    return !(lhs == rhs);
}

} // end namespace detail


namespace registration {

auto create(std::string_view name,
            std::string_view description,
            std::string_view host,
            int port,
            const Topic& topic,
            proto::Registration::Type type) -> proto::Registration
{
    auto data = proto::Registration{};
    data.set_name(name.data(), name.size());
    data.set_host(host.data(), host.size());
    data.set_description(description.data(), description.size());
    data.set_port(port);
    data.set_topic(topic.str());
    data.set_type(type);
    return data;
}


auto filter(proto::Registration::Type type) -> proto::Registration
{
    auto data = proto::Registration{};
    data.set_type(type);
    return data;
}


} // end namespace registration

} // end namespace clara::msg
