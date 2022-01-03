/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "component.hpp"

namespace clara::util {

auto make_name(std::string_view host,
               int port,
               std::string_view lang) -> std::string
{
    auto name = std::string{host};
    if (port != constants::cpp_port) {
        name.append(constants::port_sep).append(std::to_string(port));
    }
    name.append(constants::lang_sep).append(lang);
    return name;
}


auto make_name(std::string_view dpe,
               std::string_view container) -> std::string
{
    auto name = std::string{};
    name.append(dpe).append(":").append(container);
    return name;
}


auto make_name(std::string_view dpe,
               std::string_view container,
               std::string_view engine) -> std::string
{
    auto name = std::string{};
    name.append(dpe).append(":").append(container).append(":").append(engine);
    return name;
}

} // end namespace clara::util
