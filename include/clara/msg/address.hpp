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
    const std::string& host() const { return host_; }

    /// The publication port of the proxy
    int pub_port() const { return pub_port_; }

    /// The subscription port of the proxy
    int sub_port() const { return sub_port_; }

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
    const std::string& host() const { return host_; }

    /// The listening port of the registrar service
    int port() const { return port_; }

private:
    std::string host_;
    int port_;
};


inline bool operator==(const ProxyAddress& lhs, const ProxyAddress& rhs)
{
    return lhs.host() == rhs.host() &&
           lhs.pub_port() == rhs.pub_port() &&
           lhs.sub_port() == rhs.sub_port();
}

inline bool operator!=(const ProxyAddress& lhs, const ProxyAddress& rhs)
{
    return !(lhs == rhs);
}


inline bool operator==(const RegAddress& lhs, const RegAddress& rhs)
{
    return lhs.host() == rhs.host() && lhs.port() == rhs.port();
}

inline bool operator!=(const RegAddress& lhs, const RegAddress& rhs)
{
    return !(lhs == rhs);
}


std::ostream& operator<<(std::ostream& os, const ProxyAddress& a);

std::ostream& operator<<(std::ostream& os, const RegAddress& a);

} // end namespace clara::msg


namespace std {

template <>
struct hash<clara::msg::ProxyAddress>
{
    std::size_t operator()(const clara::msg::ProxyAddress& k) const
    {
        using std::hash;

        return hash<std::string>{}(k.host()) ^ (hash<int>{}(k.pub_port()) << 1);
    }
};

template <>
struct hash<clara::msg::RegAddress>
{
    std::size_t operator()(const clara::msg::RegAddress& k) const
    {
        using std::hash;

        return hash<std::string>{}(k.host()) ^ (hash<int>{}(k.port()) << 1);
    }
};

} // end namespace std

#endif // CLARA_MSG_ADDRESS_H_
