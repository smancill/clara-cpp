/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/stdlib/json_utils.hpp>

#include <clara/engine_data.hpp>

namespace {

auto json_key_error(std::string_view key, std::string_view desc)
    -> clara::stdlib::JsonError
{
    constexpr auto pre = std::string_view{"JSON key \""};
    constexpr auto sep = std::string_view{"\" "};

    auto errmsg = std::string{};
    errmsg.append(pre).append(key).append(sep).append(desc);
    return clara::stdlib::JsonError{errmsg};
}

auto get_value(const json11::Json& obj,
               std::string_view key) -> const json11::Json&
{
    using clara::stdlib::JsonError;
    if (key.empty()) {
        throw std::invalid_argument{"empty key parameter"};
    }
    const auto& val = obj[key];
    if (val.is_null()) {
        throw json_key_error(key, "is not present");
    }
    return val;
}

}


namespace clara::stdlib {

auto parse_json(const EngineData& input) -> json11::Json
{
    return parse_json(data_cast<std::string>(input));
}


auto parse_json(const std::string& str) -> json11::Json
{
    auto err = std::string{};
    auto obj = json11::Json::parse(str, err);
    if (obj.is_null()) {
        throw JsonError{err};
    }
    return obj;
}


auto has_key(const json11::Json& obj, std::string_view key) -> bool
{
    return !obj[key].is_null();
}


auto get_bool(const json11::Json& obj, std::string_view key) -> bool
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::BOOL) {
        return val.bool_value();
    }
    throw json_key_error(key, "is not a boolean");
}


auto get_int(const json11::Json& obj, std::string_view key) -> int
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::NUMBER) {
        return val.int_value();
    }
    throw json_key_error(key, "is not a number");
}


auto get_double(const json11::Json& obj, std::string_view key) -> double
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::NUMBER) {
        return val.number_value();
    }
    throw json_key_error(key, "is not a number");
}


auto get_string(const json11::Json& obj, std::string_view key)
    -> const std::string&
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::STRING) {
        return val.string_value();
    }
    throw json_key_error(key, "is not a string");
}


auto get_array(const json11::Json& obj, std::string_view key)
    -> const json11::Json&
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::ARRAY) {
        return val;
    }
    throw json_key_error(key, "is not an array");
}


auto get_object(const json11::Json& obj, std::string_view key)
    -> const json11::Json&
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::OBJECT) {
        return val;
    }
    throw json_key_error(key, "is not an object");
}


void check_json(const json11::Json& obj, const json11::Json::shape& shape)
{
    auto err = std::string{};
    if (!obj.has_shape(shape, err)) {
        throw JsonError{err};
    }
}

} // end namespace clara::stdlib
