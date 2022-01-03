/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_LOGGER_HPP
#define CLARA_LOGGER_HPP

#include "utils.hpp"

#include <iostream>
#include <mutex>

namespace clara {

namespace log {

class Logger
{
public:
    static auto instance() -> Logger*
    {
        static Logger logger;
        return &logger;
    }

public:
    template<typename... Args>
    void info(Args&&... args)
    {
        auto time = util::get_current_time();
        std::unique_lock<std::mutex> lock{m_};

        std::cout << time << ": ";
        tprintf(std::cout, std::forward<Args>(args)...);
        std::cout << std::endl;
    }

    template<typename... Args>
    void error(Args&&... args)
    {
        auto time = util::get_current_time();
        std::unique_lock<std::mutex> lock{m_};

        std::cerr << time << ": ";
        tprintf(std::cerr, std::forward<Args>(args)...);
        std::cerr << std::endl;
    }

private:
    void tprintf(std::ostream& out, const char* format)
    {
        out << format;
    }

    template<typename T, typename... Targs>
    void tprintf(std::ostream& out, const char* format, T&& value, Targs&&... Fargs)
    {
        for ( ; *format != '\0'; format++ ) {
            if ( *format == '%' ) {
                out << value;
                tprintf(out, format+2, std::forward<Targs>(Fargs)...);
                return;
            }
            out << *format;
        }
    }

private:
    std::mutex m_;
};

} // end namespace log

log::Logger* const LOGGER = log::Logger::instance();

} // end namespace clara

#endif // end of include guard: CLARA_LOGGER_HPP
