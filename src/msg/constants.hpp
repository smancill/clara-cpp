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
