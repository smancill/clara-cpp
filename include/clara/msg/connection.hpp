/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_CONNECTION_H_
#define CLARA_MSG_CONNECTION_H_

#include <clara/msg/address.hpp>
#include <clara/msg/message.hpp>

#include <memory>

namespace clara::msg {

namespace detail {

class ProxyDriver;
class RegDriver;

struct ProxyDriverDeleter
{
    void operator()(ProxyDriver* p);
};

struct RegDriverDeleter
{
    void operator()(RegDriver* p);
};

using ProxyDriverPtr = std::unique_ptr<ProxyDriver, ProxyDriverDeleter>;
using RegDriverPtr = std::unique_ptr<RegDriver, RegDriverDeleter>;


} // end namespace detail

class ConnectionPool;


template<typename A, typename U>
class ScopedConnection
{
public:
    using pointer = typename U::pointer;
    using element_type = typename U::element_type;
    using deleter = std::function<void(U&&)>;

private:
    ScopedConnection(const A& addr, U&& con, deleter&& del)
      : addr_{addr}
      , con_{std::move(con)}
      , del_{std::move(del)}
    {
        // nop
    }

public:
    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection(ScopedConnection&&) noexcept(std::is_nothrow_move_constructible_v<deleter>) = default;

    auto operator=(const ScopedConnection&) -> ScopedConnection& = delete;
    auto operator=(ScopedConnection&&) noexcept(std::is_nothrow_move_constructible_v<deleter>) -> ScopedConnection& = default;

    ~ScopedConnection()
    {
        if (con_) {
            del_(std::move(con_));
        }
    };

public:
    auto address() const -> const A&
    {
        return addr_;
    }

    auto get() const -> pointer
    {
        return con_.get();
    }

    void close()
    {
        con_.reset();
    }

private:
    auto operator*() const -> typename std::add_lvalue_reference<element_type>::type
    {
        return *con_;
    }

    auto operator->() const -> pointer
    {
        return get();
    }

    auto release() -> U
    {
        auto con = std::move(con_);
        return con;
    }

private:
    friend ConnectionPool;
    friend Actor;

    A addr_;
    U con_;
    deleter del_;
};

using ProxyConnection = ScopedConnection<ProxyAddress, detail::ProxyDriverPtr>;
using RegConnection = ScopedConnection<RegAddress, detail::RegDriverPtr>;

} // end namespace clara::msg

#endif // CLARA_MSG_CONNECTION_H_
