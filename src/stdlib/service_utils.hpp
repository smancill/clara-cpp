/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_STD_SERVICE_UTILS_HPP
#define CLARA_STD_SERVICE_UTILS_HPP

#include <clara/engine_data.hpp>

namespace clara::util {

/**
 * Sets the given engine data with an error status.
 *
 * @param output the engine data that will be returned by the service
 * @param msg a description for the error
 * @param severity the severity of the error, as a positive integer
 */
template<typename S,
         typename = std::enable_if_t<std::is_constructible_v<std::string, S>>>
void set_error(EngineData& output, S&& msg, int severity = 1)
{
    if constexpr (std::is_same_v<std::decay_t<S>, std::string_view>) {
        output.set_description(std::string{msg});
    } else {
        output.set_description(std::forward<S>(msg));
    }
    output.set_status(clara::EngineStatus::ERROR, severity);
}

} // end namespace clara::util

#endif // end of include guard: CLARA_STD_SERVICE_UTILS_HPP
