/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_ADDRESS_H_
#define CLARA_MSG_ADDRESS_H_

#include <functional>
#include <string>

namespace clara::msg {

/**
 * The network address of a proxy.
 * Proxies connect and dispatch messages between actors running on
 * a cloud of nodes. By default, proxies use localhost IP as its address,
 * the \ref constants::default_port as the PUB port, and PUB+1 as the SUB
 * port.
 */
class ProxyAddress final
{
public:
    static const int default_port = 7771;

public:
    /// Creates an address using localhost and default ports
    ProxyAddress();

    /// Creates an address using provided host and default ports
    explicit ProxyAddress(const std::string& host);

    /// Creates an address using provided host and default ports
    explicit ProxyAddress(std::string&& host);

    /// Creates an address using provided host and PUB port,
    /// with default SUB port
    ProxyAddress(const std::string& host, int pub_port);

    /// Creates an address using provided host and PUB port,
    /// with default SUB port
    ProxyAddress(std::string&& host, int pub_port);

public:
    /// The host IP of the proxy
    auto host() const -> const std::string& { return host_; }

    /// The publication port of the proxy
    auto pub_port() const -> int { return pub_port_; }

    /// The subscription port of the proxy
    auto sub_port() const -> int { return sub_port_; }

private:
    std::string host_;
    int pub_port_;
    int sub_port_;
};


/**
 * The network address of a registrar service.
 * Registration services allow discoverability of running actors.
 * By default, registrar services use localhost IP as its address,
 * and \ref constants::registrar_port as the listening port.
 */
class RegAddress final
{
public:
    static const int default_port = 8888;

public:
    /// Creates an address using localhost and default port
    RegAddress();

    /// Creates an address using provided host and default port
    explicit RegAddress(const std::string& host);

    /// Creates an address using provided host and default port
    explicit RegAddress(std::string&& host);

    /// Creates an address using provided host and port
    RegAddress(const std::string& host, int port);

    /// Creates an address using provided host and port
    RegAddress(std::string&& host, int port);

public:
    /// The host IP of the registrar service
    auto host() const -> const std::string& { return host_; }

    /// The listening port of the registrar service
    auto port() const -> int { return port_; }

private:
    std::string host_;
    int port_;
};


inline auto operator==(const ProxyAddress& lhs, const ProxyAddress& rhs) -> bool
{
    return lhs.host() == rhs.host() &&
           lhs.pub_port() == rhs.pub_port() &&
           lhs.sub_port() == rhs.sub_port();
}

inline auto operator!=(const ProxyAddress& lhs, const ProxyAddress& rhs) -> bool
{
    return !(lhs == rhs);
}


inline auto operator==(const RegAddress& lhs, const RegAddress& rhs) -> bool
{
    return lhs.host() == rhs.host() && lhs.port() == rhs.port();
}

inline auto operator!=(const RegAddress& lhs, const RegAddress& rhs) -> bool
{
    return !(lhs == rhs);
}


auto operator<<(std::ostream& os, const ProxyAddress& a) -> std::ostream&;

auto operator<<(std::ostream& os, const RegAddress& a) -> std::ostream&;

} // end namespace clara::msg


namespace std {

template <>
struct hash<clara::msg::ProxyAddress>
{
    auto operator()(const clara::msg::ProxyAddress& k) const -> std::size_t
    {
        using std::hash;

        return hash<std::string>{}(k.host()) ^ (hash<int>{}(k.pub_port()) << 1);
    }
};

template <>
struct hash<clara::msg::RegAddress>
{
    auto operator()(const clara::msg::RegAddress& k) const -> std::size_t
    {
        using std::hash;

        return hash<std::string>{}(k.host()) ^ (hash<int>{}(k.port()) << 1);
    }
};

} // end namespace std

#endif // CLARA_MSG_ADDRESS_H_
