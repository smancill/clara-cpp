/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_STD_JSON_UTILS_HPP
#define CLARA_STD_JSON_UTILS_HPP

#include <clara/third_party/json11.hpp>

#include <stdexcept>
#include <string>

namespace clara {

class EngineData;

namespace stdlib {

/**
  * A problem parsing JSON data
  */
class JsonError : public std::runtime_error
{
public:
    JsonError(const char* msg)
        : std::runtime_error{msg}
    {}

    JsonError(const std::string& msg)
        : std::runtime_error{msg}
    {}
};


auto parse_json(const EngineData& input) -> json11::Json;

auto parse_json(const std::string& str) -> json11::Json;

auto has_key(const json11::Json& obj, std::string_view key) -> bool;

auto get_bool(const json11::Json& obj, std::string_view key) -> bool;

auto get_int(const json11::Json& obj, std::string_view key) -> int;

auto get_double(const json11::Json& obj, std::string_view key) -> double;

auto get_string(const json11::Json& obj, std::string_view key) -> const std::string&;

auto get_array(const json11::Json& obj, std::string_view key) -> const json11::Json&;

auto get_object(const json11::Json& obj, std::string_view key) -> const json11::Json&;

void check_json(const json11::Json& obj, const json11::Json::shape& shape);

} // end namespace stdlib
} // end namespace clara

#endif // end of include guard: CLARA_STD_JSON_UTILS_HPP
