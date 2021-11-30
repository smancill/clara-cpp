/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "data_utils.hpp"

#include <clara/engine_data_type.hpp>

#include <clara/msg/message.hpp>


namespace clara::util {

auto build_request(const msg::Topic& topic, std::string_view data) -> msg::Message
{
    return msg::Message{topic, type::STRING.mime_type(),
                        std::vector<std::uint8_t>{data.begin(), data.end()}};
}


auto parse_message(const msg::Message& msg) -> std::string
{
    const auto& data = msg.data();
    return std::string{data.begin(), data.end()};
}

} // end namespace clara::util
