/*
 * Copyright (c) 2016.  Jefferson Lab (JLab). All rights reserved. Permission
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

#include "utils.hpp"

#include "constants.hpp"

#include <stdexcept>

namespace {

const char SEPARATOR = ':';

auto get_port(std::string_view full_name, size_t index) -> int
{
    switch (full_name[index]) {
        case 'j':
        case 'J':
            return clara::constants::java_port;
        case 'c':
        case 'C':
            return clara::constants::cpp_port;
        case 'p':
        case 'P':
            return clara::constants::python_port;
        default:
            throw std::invalid_argument{"invalid language:" + std::string{full_name}};
    }
}
}


namespace clara::msg::detail {

auto get_domain(std::string_view topic) -> std::string_view;
auto get_subject(std::string_view topic) -> std::string_view;
auto get_type(std::string_view topic) -> std::string_view;

} // end namespace clara::msg::detail


namespace clara::util {

using msg::detail::get_domain;
using msg::detail::get_subject;
using msg::detail::get_type;


auto get_dpe_name(std::string_view canonical_name) -> std::string_view
{
    return get_domain(canonical_name);
}


auto get_container_name(std::string_view canonical_name) -> std::string_view
{
    return get_subject(canonical_name);
}


auto get_container_canonical_name(std::string_view canonical_name) -> std::string_view
{
    auto first = canonical_name.find(SEPARATOR);
    if (first == decltype(canonical_name)::npos) {
        throw std::invalid_argument{"not a container or service name"};
    }
    auto second = canonical_name.find(SEPARATOR, first + 1);
    if (second == decltype(canonical_name)::npos) {
        return canonical_name;
    }
    return canonical_name.substr(0, second);
}


auto get_engine_name(std::string_view canonical_name) -> std::string_view
{
    return get_type(canonical_name);
}


auto get_dpe_host(std::string_view canonical_name) -> std::string_view
{
    auto port_sep = canonical_name.find(constants::port_sep);
    if (port_sep == decltype(canonical_name)::npos) {
        auto lang_sep = canonical_name.find(constants::lang_sep);
        return canonical_name.substr(0, lang_sep);
    }
    return canonical_name.substr(0, port_sep);
}

auto get_dpe_port(std::string_view canonical_name) -> int
{
    auto port_sep = canonical_name.find(constants::port_sep);
    auto lang_sep = canonical_name.find(constants::lang_sep);
    if (port_sep == decltype(canonical_name)::npos) {
        return get_port(canonical_name, lang_sep + 1);
    }
    auto port = std::string{canonical_name.substr(port_sep + 1, lang_sep)};
    return std::stoi(port);
}


auto get_dpe_lang(std::string_view canonical_name) -> std::string_view
{
    auto dpe_name = get_dpe_name(canonical_name);
    return dpe_name.substr(dpe_name.find(constants::lang_sep) + 1);
}


auto get_default_port(std::string_view lang) -> int
{
    return get_port(lang, 0);
}


} // end namespace clara::util
