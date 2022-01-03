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

#include <clara/msg/context.hpp>

#include "zhelper.hpp"


#include <iostream>

namespace clara::msg {

Context::Context()
  : impl_{std::make_unique<detail::Context>()}
{
    // nop
}


Context::~Context() = default;


auto Context::instance() -> std::shared_ptr<Context>
{
    static auto ctx = std::shared_ptr<Context>(new Context{});
    return ctx;
}

auto Context::create() -> std::unique_ptr<Context>
{
    return std::unique_ptr<Context>{new Context()};
}


void Context::set_io_threads(int threads)
{
    impl_->set_option(ZMQ_IO_THREADS, threads);
}


auto Context::io_threads() -> int
{
    return impl_->get_option(ZMQ_IO_THREADS);
}


void Context::set_max_sockets(int sockets)
{
    impl_->set_option(ZMQ_MAX_SOCKETS, sockets);
}


auto Context::max_sockets() -> int
{
    return impl_->get_option(ZMQ_MAX_SOCKETS);
}

} // end namespace clara::msg
