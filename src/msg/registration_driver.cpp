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

#include "registration_driver.hpp"

#include "constants.hpp"
#include "zhelper.hpp"

#include <iostream>
#include <tuple>

namespace {

auto make_tie(const clara::msg::proto::Registration& reg, int& port, int& owner)
{
    port = reg.port();
    owner = int{reg.ownertype()};
    return std::tie(reg.name(), reg.domain(), reg.subject(), reg.type(),
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

Request::Request(std::string_view topic,
                 std::string_view sender,
                 const proto::Registration& data)
  : msg_{zmq::message_t{topic},
         zmq::message_t{sender},
         zmq::message_t{data.SerializeAsString()}}
{ }


Request::Request(std::string_view topic,
                 std::string_view sender,
                 std::string_view text)
  : msg_{zmq::message_t{topic},
         zmq::message_t{sender},
         zmq::message_t{text}}
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



Response::Response(std::string_view topic,
                   std::string_view sender)
{
    msg_.reserve(n_fields);
    msg_.emplace_back(topic);
    msg_.emplace_back(sender);
    msg_.emplace_back(constants::success);
}


Response::Response(std::string_view topic,
                   std::string_view sender,
                   const RegDataSet& data)
{
    msg_.reserve(n_fields + data.size());
    msg_.emplace_back(topic);
    msg_.emplace_back(sender);
    msg_.emplace_back(constants::success);
    for (const auto& reg : data) {
        msg_.emplace_back(reg.SerializeAsString());
    }
}


Response::Response(std::string_view topic,
                   std::string_view sender,
                   std::string_view error_msg)
{
    msg_.reserve(n_fields);
    msg_.emplace_back(topic);
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


void RegDriver::add(const proto::Registration& data, bool is_publisher)
{
    auto topic = is_publisher ? constants::register_publisher
                              : constants::register_subscriber;
    auto req = Request{topic, data.name(), data};
    request(req, constants::register_request_timeout);
}


void RegDriver::remove(const proto::Registration& data, bool is_publisher)
{
    auto topic = is_publisher ? constants::remove_publisher
                              : constants::remove_subscriber;
    auto req = Request{topic, data.name(), data};
    request(req, constants::remove_request_timeout);
}


void RegDriver::remove_all(std::string_view sender, std::string_view host)
{
    auto req = Request{constants::remove_all_registration, sender, host};
    request(req, constants::remove_request_timeout);
}


auto RegDriver::find(const proto::Registration& data, bool is_publisher) -> RegDataSet
{
    auto topic = is_publisher ? constants::find_publisher
                              : constants::find_subscriber;
    auto req = Request{topic, data.name(), data};
    auto res = request(req, constants::find_request_timeout);
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
            bool is_publisher) -> proto::Registration
{
    auto data_type = is_publisher
            ? proto::Registration::PUBLISHER
            : proto::Registration::SUBSCRIBER;
    auto data = proto::Registration{};
    data.set_name(name.data(), name.size());
    data.set_host(host.data(), host.size());
    data.set_description(description.data(), description.size());
    data.set_port(port);
    data.set_domain(topic.domain());
    data.set_subject(topic.subject());
    data.set_type(topic.type());
    data.set_ownertype(data_type);
    return data;
}

} // end namespace registration

} // end namespace clara::msg
