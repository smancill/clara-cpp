/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_UTILS_HPP
#define CLARA_UTILS_HPP

#include <string>
#include <string_view>


namespace clara::msg::util {

auto get_current_time() -> std::string;

}

namespace clara::util {

using msg::util::get_current_time;  // NOLINT

auto get_dpe_name(std::string_view canonical_name) -> std::string_view;

auto get_container_name(std::string_view canonical_name) -> std::string_view;

auto get_container_canonical_name(std::string_view canonical_name) -> std::string_view;

auto get_engine_name(std::string_view canonical_name) -> std::string_view;

auto get_dpe_host(std::string_view canonical_name) -> std::string_view;

auto get_dpe_port(std::string_view canonical_name) -> int;

auto get_dpe_lang(std::string_view canonical_name) -> std::string_view;

auto get_default_port(std::string_view lang) -> int;

} // end namespace clara::util

#endif // end of include guard: CLARA_UTILS_HPP
