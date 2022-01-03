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

#include <clara/msg/address.hpp>

#include <clara/msg/utils.hpp>

#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace {

constexpr auto privileged_ports = 1024;
constexpr auto max_port_number = UINT16_MAX;

inline auto to_sub_port(int pub_port) -> int
{
    return pub_port + 1;
}

template<typename T, typename R = std::decay_t<T>>
inline auto to_addr(T&& host) -> R
{
    using namespace clara::msg;

    if (util::is_ipaddr(host)) {
        return R{std::forward<T>(host)};
    }
    return std::string{util::to_host_addr(host)};
}

inline auto check(int port) -> int
{
    if (port < privileged_ports || port > max_port_number) {
        throw std::invalid_argument{"invalid port: " + std::to_string(port)};
    };
    return port;
}

} // end empty namespace


namespace clara::msg {

ProxyAddress::ProxyAddress()
  : host_{util::localhost()}
  , pub_port_{default_port}
  , sub_port_{to_sub_port(default_port)}
{ }


ProxyAddress::ProxyAddress(const std::string& host)
  : host_{to_addr(host)}
  , pub_port_{default_port}
  , sub_port_{to_sub_port(default_port)}
{ }


ProxyAddress::ProxyAddress(std::string&& host)
  : host_{to_addr(std::move(host))}
  , pub_port_{default_port}
  , sub_port_{to_sub_port(default_port)}
{ }


ProxyAddress::ProxyAddress(const std::string& host, int pub_port)
  : host_{to_addr(host)}
  , pub_port_{check(pub_port)}
  , sub_port_{check(to_sub_port(pub_port))}
{ }


ProxyAddress::ProxyAddress(std::string&& host, int pub_port)
  : host_{to_addr(std::move(host))}
  , pub_port_{check(pub_port)}
  , sub_port_{check(to_sub_port(pub_port))}
{ }


RegAddress::RegAddress()
  : host_{util::localhost()}
  , port_{default_port}
{ }


RegAddress::RegAddress(const std::string& host)
  : host_{to_addr(host)}
  , port_{default_port}
{ }


RegAddress::RegAddress(std::string&& host)
  : host_{to_addr(std::move(host))}
  , port_{default_port}
{ }


RegAddress::RegAddress(const std::string& host, int port)
  : host_{to_addr(host)}
  , port_{check(port)}
{ }


RegAddress::RegAddress(std::string&& host, int port)
  : host_{to_addr(std::move(host))}
  , port_{check(port)}
{ }


auto operator<<(std::ostream& os, const ProxyAddress& a) -> std::ostream&
{
    return os << a.host() << ":" << a.pub_port();
}


auto operator<<(std::ostream& os, const RegAddress& a) -> std::ostream&
{
    return os << a.host() << ":" << a.port();
}

} // end namespace clara::msg
