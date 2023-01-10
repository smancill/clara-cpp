/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_PROTO_DATA_H_
#define CLARA_MSG_PROTO_DATA_H_

#include <clara/msg/mimetype.hpp>

#include <google/protobuf/wrappers.pb.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace clara::msg::proto {

// clang-format off
namespace detail {

using buffer_t = std::vector<std::uint8_t>;


template <typename P, typename T>
inline auto serialize_proto_value(const T& value) -> buffer_t
{
    auto proto = P{};
    proto.set_value(value);
    auto buffer = buffer_t(proto.ByteSizeLong());
    proto.SerializeToArray(buffer.data(), static_cast<int>(buffer.size()));
    return buffer;
}


template <typename P, typename V>
inline auto parse_proto_value(const V& data)
{
    auto p = P{};
    p.ParseFromArray(data.data(), data.size());
    return p.value();
}


template <typename T>
inline auto serialize_value(T&& value) -> buffer_t
{
    using U = std::decay_t<T>;

    if constexpr(std::is_constructible_v<std::string_view, T>) {
        auto v = std::string_view{value};
        return {v.begin(), v.end()};
    } else if constexpr(std::is_same_v<U, std::int32_t>) {
        return serialize_proto_value<google::protobuf::Int32Value>(value);
    } else if constexpr(std::is_same_v<U, std::int64_t>) {
        return serialize_proto_value<google::protobuf::Int64Value>(value);
    } else if constexpr(std::is_same_v<U, float>) {
        return serialize_proto_value<google::protobuf::FloatValue>(value);
    } else if constexpr(std::is_same_v<U, double>) {
        return serialize_proto_value<google::protobuf::DoubleValue>(value);
    } else if constexpr(std::is_same_v<U, buffer_t>) {
        return std::forward<T>(value);
    } else {
        static_assert(sizeof(T) == 0, "Unsupported type");
    }
}


template <typename T, typename V,
          typename = std::enable_if_t<std::is_same_v<std::decay_t<V>, buffer_t>>>
inline auto parse_value(V&& buffer) -> T
{
    if constexpr(std::is_same_v<T, std::string>) {
        return std::string{std::begin(buffer), std::end(buffer)};
    } else if constexpr(std::is_same_v<T, std::int32_t>) {
        return parse_proto_value<google::protobuf::Int32Value>(buffer);
    } else if constexpr(std::is_same_v<T, std::int64_t>) {
        return parse_proto_value<google::protobuf::Int64Value>(buffer);
    } else if constexpr(std::is_same_v<T, float>) {
        return parse_proto_value<google::protobuf::FloatValue>(buffer);
    } else if constexpr(std::is_same_v<T, double>) {
        return parse_proto_value<google::protobuf::DoubleValue>(buffer);
    } else if constexpr(std::is_same_v<T, buffer_t>) {
        return std::forward<V>(buffer);
    } else {
        static_assert(sizeof(T) == 0, "Unsupported type");
    }
}


template <typename T>
inline auto get_mimetype() -> std::string_view
{
    if constexpr(std::is_constructible_v<std::string, T>) {
        return mimetype::string;
    } else if constexpr(std::is_same_v<T, std::int32_t>) {
        return mimetype::int32_number;
    } else if constexpr(std::is_same_v<T, std::int64_t>) {
        return mimetype::int64_number;
    } else if constexpr(std::is_same_v<T, float>) {
        return mimetype::float_number;
    } else if constexpr(std::is_same_v<T, double>) {
        return mimetype::double_number;
    } else if constexpr(std::is_same_v<T, buffer_t>) {
        return mimetype::bytes;
    } else {
        static_assert(sizeof(T) == 0, "Unsupported data type");
    }
}

} // end namespace detail
// clang-format on

} // end namespace clara::msg::proto

#endif // CLARA_MSG_PROTO_DATA_H_
