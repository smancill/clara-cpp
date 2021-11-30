/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_CONSTANTS_H_
#define CLARA_MSG_CONSTANTS_H_

#include <string_view>

namespace clara::msg::constants {

using namespace std::literals::string_view_literals;

// clang-format off
constexpr auto success = "success"sv;

constexpr auto register_publisher = "registerPublisher"sv;
constexpr auto register_subscriber = "registerSubscriber"sv;

constexpr auto remove_publisher = "removePublisherRegistration"sv;
constexpr auto remove_subscriber = "removeSubscriberRegistration"sv;
constexpr auto remove_all_registration = "removeAllRegistration"sv;

constexpr auto find_publisher = "findPublisher"sv;
constexpr auto find_subscriber = "findSubscriber"sv;

constexpr auto register_request_timeout = 3000;
constexpr auto remove_request_timeout = 3000;
constexpr auto find_request_timeout = 3000;

constexpr auto ctrl_topic = "clara:msg:control"sv;
constexpr auto ctrl_connect = "pub"sv;
constexpr auto ctrl_subscribe = "sub"sv;
constexpr auto ctrl_reply = "rep"sv;
// clang-format on

} // end namespace clara::msg::constants

#endif // CLARA_MSG_CONSTANTS_H_
