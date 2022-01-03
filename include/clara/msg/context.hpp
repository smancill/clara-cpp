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

#ifndef CLARA_MSG_CONTEXT_H_
#define CLARA_MSG_CONTEXT_H_

#include <memory>

namespace clara::msg {

namespace detail {
class Context;
} // end namespace detail

namespace sys {
class Proxy;
} // end namespace sys


/**
 * Singleton class that provides unique 0MQ context for entire process.
 */
class Context
{
public:
    /**
     * Returns the global singleton context.
     */
    static auto instance() -> std::shared_ptr<Context>;

    /**
     * Creates a new context.
     */
    static auto create() -> std::unique_ptr<Context>;

public:
    /**
     * Sets the size of the 0MQ thread pool to handle I/O operations.
     */
    void set_io_threads(int threads);

    /**
     * Gets the size of the 0MQ thread pool to handle I/O operations.
     */
    auto io_threads() -> int;

    /**
     * Sets the maximum number of sockets allowed on the context.
     */
    void set_max_sockets(int sockets);

    /**
     * Gets the maximum number of sockets allowed on the context.
     */
    auto max_sockets() -> int;

private:
    Context();

public:
    Context(const Context&) = delete;
    Context(Context&&) = delete;

    auto operator=(const Context&) -> Context& = delete;
    auto operator=(Context&&) -> Context& = delete;

    ~Context();

private:
    std::unique_ptr<detail::Context> impl_;

    friend class ConnectionPool;
    friend class sys::Proxy;
};

} // end namespace clara::msg

#endif // CLARA_MSG_CONTEXT_H_
