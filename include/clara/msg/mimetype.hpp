/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_MIMETYPE_H_
#define CLARA_MSG_MIMETYPE_H_

#include <string>

/**
 * Identifiers for base data types.
 * Most of these types represent the values that can be stored in
 * proto::Data objects, and are set automatically
 * when a message is created with \ref make_message.
 * For more information about the Protocol Buffer value types, check
 * [here](https://developers.google.com/protocol-buffers/docs/proto#scalar).
 *
 * Clients must define their own strings for custom data types.
 * The identifier should be used to check that the message contains
 * the expected data.
 */
namespace clara::msg::mimetype {

using namespace std::literals::string_view_literals;

// clang-format off
constexpr auto int32_number = "binary/int32"sv;     ///< Integer of 32 bits.
constexpr auto int64_number = "binary/int64"sv;     ///< Integer of 64 bits.
constexpr auto float_number = "binary/float"sv;     ///< Single precision floating point type.
constexpr auto double_number = "binary/double"sv;   ///< Double precision floating point type.
constexpr auto string = "text/string"sv;            ///< An UTF-8 encoded or 7-bit ASCII text.
constexpr auto bytes = "binary/bytes"sv;            ///< An arbitrary sequence of bytes.

constexpr auto plain_data = "binary/native"sv;                  ///< A serialized \ref clara::msg::proto::Data "Data" object
constexpr auto java_object = "binary/java"sv;                   ///< A serialized Java object
constexpr auto cpp_object = "binary/cpp"sv;                     ///< A serialized C++ object
constexpr auto python_object = "binary/python"sv;               ///< A serialized Python object
// clang-format on

} // end namespace clara::msg::mimetype

#endif // CLARA_MSG_MIMETYPE_H_
