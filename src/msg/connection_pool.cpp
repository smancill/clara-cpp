/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/connection_pool.hpp>

#include <clara/msg/context.hpp>

#include "connection_driver.hpp"
#include "registration_driver.hpp"

#include <queue>
#include <unordered_map>


namespace clara::msg {

namespace detail {

void ProxyDriverDeleter::operator()(ProxyDriver* p)
{
    delete p;
}

void RegDriverDeleter::operator()(RegDriver* p)
{
    delete p;
}

} // end namespace detail


template <typename A, typename U>
class ConnectionPool::ConnectionCache
{
    using ConnectionQueue = std::queue<U>;
    using ConnectionMap = std::unordered_map<A, ConnectionQueue>;

public:
    auto get(const A& addr) -> U
    {
        U up;
        auto it = cache_.find(addr);
        if (it != cache_.end()) {
            auto& q = it->second;
            if (!q.empty()) {
                up = std::move(q.front());
                q.pop();
            }
        }
        return up;
    }

    void set(const A& addr, U&& con)
    {
        auto it = cache_.find(addr);
        if (it == cache_.end()) {
            auto qp = cache_.emplace(addr, ConnectionQueue{});
            it = qp.first;
        }
        it->second.push(std::move(con));
    }

private:
    ConnectionMap cache_;
};


ConnectionPool::ConnectionPool()
  : ConnectionPool{Context::instance()}
{ }


ConnectionPool::ConnectionPool(std::shared_ptr<Context> ctx)
  : ctx_{std::move(ctx)}
  , setup_{std::make_shared<ConnectionSetup>()}
  , proxy_cache_{std::make_unique<ProxyDriverCache>()}
  , reg_cache_{std::make_unique<RegDriverCache>()}
{ }


ConnectionPool::ConnectionPool(ConnectionPool&&) noexcept = default;

auto ConnectionPool::operator=(ConnectionPool&&) noexcept -> ConnectionPool& = default;

ConnectionPool::~ConnectionPool() = default;


auto ConnectionPool::get_connection(const ProxyAddress& addr) -> ProxyConnection
{
    auto con = proxy_cache_->get(addr);
    if (!con) {
        con = create_connection(addr);
    }
    auto del = [this](detail::ProxyDriverPtr&& c) {
        proxy_cache_->set(c->address(), std::move(c));
    };
    return {ProxyAddress{addr}, std::move(con), std::move(del)};
}


void ConnectionPool::set_default_setup(std::unique_ptr<ConnectionSetup> setup)
{
    setup_ = std::move(setup);
}


auto ConnectionPool::get_connection(const RegAddress& addr) -> RegConnection
{
    auto con = reg_cache_->get(addr);
    if (!con) {
        con = create_connection(addr);
    }
    auto del = [this](detail::RegDriverPtr&& c) {
        reg_cache_->set(c->address(), std::move(c));
    };
    return {RegAddress{addr}, std::move(con), std::move(del)};
}


auto ConnectionPool::create_connection(const ProxyAddress& addr)
    -> detail::ProxyDriverPtr
{
    auto con = detail::ProxyDriverPtr{new detail::ProxyDriver(*ctx_->impl_, addr, setup_)};
    con->connect();
    return con;
}


auto ConnectionPool::create_connection(const RegAddress& addr)
    -> detail::RegDriverPtr
{
    return detail::RegDriverPtr{new detail::RegDriver(*ctx_->impl_, addr)};
}

} // end namespace clara::msg
