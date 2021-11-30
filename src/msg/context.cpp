/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
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
