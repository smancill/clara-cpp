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
 *
 * These types represent simple types that can be stored in a message
 * with the \ref make_message helper function.
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
// clang-format on

} // end namespace clara::msg::mimetype

#endif // CLARA_MSG_MIMETYPE_H_
