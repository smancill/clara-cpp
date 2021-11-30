/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_JSON_UTILS_HPP
#define CLARA_JSON_UTILS_HPP

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string>

namespace clara::util {

using Buffer = rapidjson::StringBuffer;
using Writer = rapidjson::Writer<rapidjson::StringBuffer>;

inline void put(Writer& writer, std::string_view key, std::string_view value)
{
    writer.Key(key.data(), key.size());
    writer.String(value.data(), value.size());
}

inline void put(Writer& writer, std::string_view key, int value)
{
    writer.Key(key.data(), key.size());
    writer.Int(value);
}


inline void put(Writer& writer, std::string_view key, long value)
{
    writer.Key(key.data(), key.size());
    writer.Int64(value);
}


inline void put(Writer& writer, std::string_view key, double value)
{
    writer.Key(key.data(), key.size());
    writer.Double(value);
}

} // end namespace clara::util

#endif // end of include guard: CLARA_JSON_UTILS_HPP
