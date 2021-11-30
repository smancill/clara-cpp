/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/connection_setup.hpp>

#include <zmq.h>

#include <cerrno>
#include <stdexcept>

namespace clara::msg {

void SocketSetup::set_option(int opt, const void* val, std::size_t val_len)
{
    int rc = zmq_setsockopt(socket_, opt, val, val_len);
    if (rc != 0) {
        throw std::runtime_error{zmq_strerror(errno)};
    }
}


void SocketSetup::get_option(int opt, void* val, std::size_t* val_len) const
{
    int rc = zmq_getsockopt(socket_, opt, val, val_len);
    if (rc != 0) {
        throw std::runtime_error{zmq_strerror(errno)};
    }
}


auto SocketSetup::type() const -> int
{
    return get_option<int>(ZMQ_TYPE);
}

} // end namespace clara::msg
