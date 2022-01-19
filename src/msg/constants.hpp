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

constexpr auto reg_add = "register"sv;
constexpr auto reg_remove = "remove"sv;
constexpr auto reg_remove_all = "remove_all"sv;

constexpr auto reg_find_matching = "find_matching"sv;

constexpr auto reg_add_timeout = 3000;
constexpr auto reg_remove_timeout = 3000;
constexpr auto reg_find_timeout = 3000;

constexpr auto ctrl_topic = "clara:msg:control"sv;
constexpr auto ctrl_connect = "pub"sv;
constexpr auto ctrl_subscribe = "sub"sv;
constexpr auto ctrl_reply = "rep"sv;
// clang-format on

} // end namespace clara::msg::constants

#endif // CLARA_MSG_CONSTANTS_H_
