/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_PROTO_DATA_H_
#define CLARA_MSG_PROTO_DATA_H_

#include "data.pb.h"

#include <clara/msg/mimetype.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace clara::msg::proto {

/**
 * \class Data
 * \brief Simple data for pub/sub communications
 *        (simple types or array of simple types).
 *
 * On pub/sub communications, data is sent serialized.
 * This class helps to publish simple types (integers, floats or strings),
 * or arrays of simple types, by using protocol buffers serialization.
 * For more information about the Protocol Buffer value types, check
 * [here](https://developers.google.com/protocol-buffers/docs/proto#scalar).
 * Use the proper accessor to set and access the required type(s).
 *
 * Normally, \ref make_message, or \ref make_data and
 * \ref serialize_data should be used to construct messages with simple
 * data. Manual creation of objects of this class should be rare.
 * For more complex data types, the client should provide its own objects
 * and serialization routines.
 */

// clang-format off
namespace detail {

using buffer_t = std::vector<std::uint8_t>;


inline auto serialize_data(const Data& data) -> buffer_t
{
    auto buffer = buffer_t(data.ByteSizeLong());
    data.SerializeToArray(buffer.data(), static_cast<int>(buffer.size()));
    return buffer;
}


inline auto parse_data(const buffer_t& buffer) -> Data
{
    auto data = Data{};
    data.ParseFromArray(buffer.data(), static_cast<int>(buffer.size()));
    return data;
}


template <typename T>
inline auto serialize_value(T&& value) -> buffer_t
{
    using U = std::decay_t<T>;

    if constexpr(std::is_same_v<U, std::string>) {
        auto data = Data{};
        data.set_string(std::forward<T>(value));
        return serialize_data(data);
    } else if constexpr(std::is_constructible_v<std::string, T>) {
        auto data = Data{};
        data.set_string(std::string{std::forward<T>(value)});
        return serialize_data(data);
    } else if constexpr(std::is_same_v<U, std::int32_t>) {
        auto data = Data{};
        data.set_flsint32(value);
        return serialize_data(data);
    } else if constexpr(std::is_same_v<U, std::int64_t>) {
        auto data = Data{};
        data.set_flsint64(value);
        return serialize_data(data);
    } else if constexpr(std::is_same_v<U, float>) {
        auto data = Data{};
        data.set_float_(value);
        return serialize_data(data);
    } else if constexpr(std::is_same_v<U, double>) {
        auto data = Data{};
        data.set_double_(value);
        return serialize_data(data);
    } else {
        static_assert(sizeof(T) == 0, "Unsupported type");
    }
}


template <typename T, typename V,
          typename = std::enable_if_t<std::is_same_v<std::decay_t<V>, buffer_t>>>
inline auto parse_value(V&& buffer) -> T
{
    if constexpr(std::is_same_v<T, std::string>) {
        auto data = parse_data(buffer);
        return data.string();
    } else if constexpr(std::is_same_v<T, std::int32_t>) {
        auto data = parse_data(buffer);
        return data.flsint32();
    } else if constexpr(std::is_same_v<T, std::int64_t>) {
        auto data = parse_data(buffer);
        return data.flsint64();
    } else if constexpr(std::is_same_v<T, float>) {
        auto data = parse_data(buffer);
        return data.float_();
    } else if constexpr(std::is_same_v<T, double>) {
        auto data = parse_data(buffer);
        return data.double_();
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


/**
 * Serializes the given %Data into a buffer.
 * The returned buffer should be used to create a Message,
 * when more control is needed over the data and/or the metadata.
 * In that case, the data type should be set carefully.
 * Otherwise, it is simpler to use \ref make_message.
 *
 * \param data the protobuf object
 * \return the serialized data
 */
inline auto serialize_data(const Data& data) -> std::vector<std::uint8_t>
{
    return detail::serialize_data(data);
}

/**
 * Deserializes the %Data from the given buffer.
 * Use this to get the %Data object when the buffer is not in a message
 * for some reason.
 * Otherwise, it is simpler to use \ref parse_message.
 *
 * \param buffer the serialized data
 * \return the deserialized %Data object
 */
inline auto parse_data(const std::vector<std::uint8_t>& buffer) -> Data
{
    return detail::parse_data(buffer);
}


inline auto operator==(const Data& lhs, const Data& rhs) -> bool
{
    return lhs.SerializeAsString() == rhs.SerializeAsString();
}

inline auto operator!=(const Data& lhs, const Data& rhs) -> bool
{
    return !(lhs == rhs);
}

} // end namespace clara::msg::proto

#endif // CLARA_MSG_PROTO_DATA_H_
