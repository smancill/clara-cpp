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
constexpr auto single_sint32 = "binary/sint32"sv;               ///< Signed integer. Uses variable-length encoding.
constexpr auto single_sint64 = "binary/sint64"sv;               ///< Signed integer. Uses variable-length encoding.
constexpr auto single_sfixed32 = "binary/sfixed32"sv;           ///< Signed integer. Always use 4 bytes.
constexpr auto single_sfixed64 = "binary/sfixed64"sv;           ///< Signed integer. Always use 8 bytes.
constexpr auto single_float = "binary/float"sv;                 ///< Single precision floating point type.
constexpr auto single_double = "binary/double"sv;               ///< Double precision floating point type.
constexpr auto single_string = "text/string"sv;                 ///< An UTF-8 encoded or 7-bit ASCII text.
constexpr auto bytes = "binary/bytes"sv;                        ///< An arbitrary sequence of bytes.

constexpr auto array_sint32 = "binary/array-sint32"sv;          ///< Repeated signed integers. Uses variable-length encoding.
constexpr auto array_sint64 = "binary/array-sint64"sv;          ///< Repeated signed integers. Uses variable-length encoding.
constexpr auto array_sfixed32 = "binary/array-sfixed32"sv;      ///< Repeated signed integers. Always use 4 bytes.
constexpr auto array_sfixed64 = "binary/array-sfixed32"sv;      ///< Repeated signed integers. Always use 8 bytes.
constexpr auto array_float = "binary/array-float"sv;            ///< Repeated single precision floating point types.
constexpr auto array_double = "binary/array-double"sv;          ///< Repeated double precision floating point types.
constexpr auto array_string = "binary/array-string"sv;          ///< Repeated UTF-8 encoded or 7-bit ASCII texts.
constexpr auto array_bytes = "binary/array-bytes"sv;            ///< Repeated arbitrary sequences of bytes.

constexpr auto plain_data = "binary/native"sv;                  ///< A serialized \ref clara::msg::proto::Data "Data" object
constexpr auto java_object = "binary/java"sv;                   ///< A serialized Java object
constexpr auto cpp_object = "binary/cpp"sv;                     ///< A serialized C++ object
constexpr auto python_object = "binary/python"sv;               ///< A serialized Python object
// clang-format on

} // end namespace clara::msg::mimetype

#endif // CLARA_MSG_MIMETYPE_H_
