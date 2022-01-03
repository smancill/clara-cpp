/*
 * Copyright (c) 2017.  Jefferson Lab (JLab). All rights reserved. Permission
 * to use, copy, modify, and distribute  this software and its documentation for
 * educational, research, and not-for-profit purposes, without fee and without a
 * signed licensing agreement.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY,
 * PROVIDED HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * This software was developed under the United States Government license.
 * For more information contact author at gurjyan@jlab.org
 * Department of Experimental Nuclear Physics, Jefferson Lab.
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
