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

template <typename T>
inline void set_value(Data& data, T&& value)
{
    using U = std::decay_t<T>;

    if constexpr(std::is_same_v<U, std::string>) {
        data.set_string(std::forward<T>(value));
    } else if constexpr(std::is_constructible_v<std::string, T>) {
        data.set_string(std::string{std::forward<T>(value)});
    } else if constexpr(std::is_same_v<U, std::int32_t>) {
        data.set_flsint32(value);
    } else if constexpr(std::is_same_v<U, std::int64_t>) {
        data.set_flsint64(value);
    } else if constexpr(std::is_same_v<U, float>) {
        data.set_float_(value);
    } else if constexpr(std::is_same_v<U, double>) {
        data.set_double_(value);
    } else {
        static_assert(sizeof(T) == 0, "Unsupported type");
    }
}


template <typename T>
inline auto get_value(const Data& data) -> T
{
    if constexpr(std::is_same_v<T, std::string>) {
        return data.string();
    } else if constexpr(std::is_same_v<T, std::int32_t>) {
        return data.flsint32();
    } else if constexpr(std::is_same_v<T, std::int64_t>) {
        return data.flsint64();
    } else if constexpr(std::is_same_v<T, float>) {
        return data.float_();
    } else if constexpr(std::is_same_v<T, double>) {
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
    } else if constexpr(std::is_same_v<T, std::vector<std::uint8_t>>) {
        return mimetype::bytes;
    } else {
        static_assert(sizeof(T) == 0, "Unsupported data type");
    }
}

} // end namespace detail
// clang-format on


/**
 * Creates a %Data object with a single value of type T.
 * The proper field to be set will be deduced from the parameter type.
 *
 * The object should be serialized in order to create a Message.
 * This is useful when more control is needed over the metadata.
 * In that case, the data type should be set carefully.
 * Otherwise, it is simpler to use \ref make_message.
 *
 * \tparam T a type that can be set on proto::Data objects
 * \param data the value to be set in the created object
 * \return the created %Data object with only the value of type T set
 */
template<typename T>
inline auto make_data(T&& data) -> Data
{
    auto xdata = Data{};
    detail::set_value(xdata, std::forward<T>(data));
    return xdata;
}

/**
 * Reads the value of type T from the given %Data object.
 * The proper field will be deduced from the return type.
 *
 * With all probability \ref parse_message could be used instead of this.
 *
 * \tparam T a type that can be get from proto::Data objects
 * \param data the protobuf object
 * \return the value of type T in the data
 */
template<typename T>
inline auto parse_data(const Data& data) -> T
{
    return detail::get_value<T>(data);
}

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
    auto buffer = std::vector<std::uint8_t>(data.ByteSizeLong());
    data.SerializeToArray(buffer.data(), static_cast<int>(buffer.size()));
    return buffer;
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
    auto xdata = Data{};
    xdata.ParseFromArray(buffer.data(), static_cast<int>(buffer.size()));
    return xdata;
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
