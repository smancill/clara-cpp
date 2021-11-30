/*
 * Copyright (C) 2015. Jefferson Lab, xMsg framework (JLAB). All Rights Reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research, and not-for-profit purposes,
 * without fee and without a signed licensing agreement.
 *
 * Contact Vardan Gyurjyan
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
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

// clang-format off
inline const std::string single_sint32 = "binary/sint32";          ///< Signed integer. Uses variable-length encoding.
inline const std::string single_sint64 = "binary/sint64";          ///< Signed integer. Uses variable-length encoding.
inline const std::string single_sfixed32 = "binary/sfixed32";      ///< Signed integer. Always use 4 bytes.
inline const std::string single_sfixed64 = "binary/sfixed64";      ///< Signed integer. Always use 8 bytes.
inline const std::string single_float = "binary/float";            ///< Single precision floating point type.
inline const std::string single_double = "binary/double";          ///< Double precision floating point type.
inline const std::string single_string = "text/string";            ///< An UTF-8 encoded or 7-bit ASCII text.
inline const std::string bytes = "binary/bytes";                   ///< An arbitrary sequence of bytes.

inline const std::string array_sint32 = "binary/array-sint32";     ///< Repeated signed integers. Uses variable-length encoding.
inline const std::string array_sint64 = "binary/array-sint64";     ///< Repeated signed integers. Uses variable-length encoding.
inline const std::string array_sfixed32 = "binary/array-sfixed32"; ///< Repeated signed integers. Always use 4 bytes.
inline const std::string array_sfixed64 = "binary/array-sfixed32"; ///< Repeated signed integers. Always use 8 bytes.
inline const std::string array_float = "binary/array-float";       ///< Repeated single precision floating point types.
inline const std::string array_double = "binary/array-double";     ///< Repeated double precision floating point types.
inline const std::string array_string = "binary/array-string";     ///< Repeated UTF-8 encoded or 7-bit ASCII texts.
inline const std::string array_bytes = "binary/array-bytes";       ///< Repeated arbitrary sequences of bytes.

inline const std::string plain_data = "binary/native";             ///< A serialized \ref clara::msg::proto::Data "Data" object
inline const std::string java_object = "binary/java";              ///< A serialized Java object
inline const std::string cpp_object = "binary/cpp";                ///< A serialized C++ object
inline const std::string python_object = "binary/python";          ///< A serialized Python object
// clang-format on

} // end namespace clara::msg::mimetype

#endif // CLARA_MSG_MIMETYPE_H_
