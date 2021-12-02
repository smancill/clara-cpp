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

#ifndef CLARA_STD_JSON_UTILS_HPP
#define CLARA_STD_JSON_UTILS_HPP

#include <clara/third_party/json11.hpp>

#include <stdexcept>
#include <string>

namespace clara {

class EngineData;

namespace stdlib {

/**
  * A problem parsing JSON data
  */
class JsonError : public std::runtime_error
{
public:
    JsonError(const char* msg)
        : std::runtime_error{msg}
    {}

    JsonError(const std::string& msg)
        : std::runtime_error{msg}
    {}
};


auto parse_json(const EngineData& input) -> json11::Json;

auto parse_json(const std::string& str) -> json11::Json;

auto has_key(const json11::Json& obj, std::string_view key) -> bool;

auto get_bool(const json11::Json& obj, std::string_view key) -> bool;

auto get_int(const json11::Json& obj, std::string_view key) -> int;

auto get_double(const json11::Json& obj, std::string_view key) -> double;

auto get_string(const json11::Json& obj, std::string_view key) -> const std::string&;

auto get_array(const json11::Json& obj, std::string_view key) -> const json11::Json&;

auto get_object(const json11::Json& obj, std::string_view key) -> const json11::Json&;

void check_json(const json11::Json& obj, const json11::Json::shape& shape);

} // end namespace stdlib
} // end namespace clara

#endif // end of include guard: CLARA_STD_JSON_UTILS_HPP
