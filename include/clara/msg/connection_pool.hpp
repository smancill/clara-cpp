/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_CONNECTION_POOL_H_
#define CLARA_MSG_CONNECTION_POOL_H_

#include <clara/msg/connection.hpp>
#include <clara/msg/connection_setup.hpp>
#include <clara/msg/context.hpp>

#include <memory>

namespace clara::msg {

class ConnectionPool
{
public:
    ConnectionPool();
    explicit ConnectionPool(std::shared_ptr<Context> ctx);

    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool(ConnectionPool&& rhs) noexcept;

    auto operator=(const ConnectionPool&) -> ConnectionPool& = delete;
    auto operator=(ConnectionPool&& rhs) noexcept -> ConnectionPool&;

    virtual ~ConnectionPool();

public:
    auto get_connection(const ProxyAddress& addr) -> ProxyConnection;

    auto get_connection(const RegAddress& addr) -> RegConnection;

public:
    void set_default_setup(std::unique_ptr<ConnectionSetup> setup);

private:
    virtual auto create_connection(const ProxyAddress& addr) -> detail::ProxyDriverPtr;
    virtual auto create_connection(const RegAddress& addr) -> detail::RegDriverPtr;

private:
    template<typename A, typename U>
    class ConnectionCache;

    using ProxyDriverCache = ConnectionCache<ProxyAddress, detail::ProxyDriverPtr>;
    using RegDriverCache = ConnectionCache<RegAddress, detail::RegDriverPtr>;

    std::shared_ptr<Context> ctx_;
    std::shared_ptr<ConnectionSetup> setup_;

    std::unique_ptr<ProxyDriverCache> proxy_cache_;
    std::unique_ptr<RegDriverCache> reg_cache_;
};

} // end namespace clara::msg

#endif // CLARA_MSG_CONNECTION_POOL_H_
