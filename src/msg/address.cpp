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

inline bool invalid_port(int port)
{
    return port < privileged_ports || port > max_port_number;
}

inline int default_sub_port(int pub_port)
{
    return pub_port + 1;
}

} // end empty namespace


namespace clara::msg {

ProxyAddress::ProxyAddress()
  : ProxyAddress{util::localhost()}
{ }


ProxyAddress::ProxyAddress(const std::string& host)
  : ProxyAddress{host, default_port}
{ }


ProxyAddress::ProxyAddress(const std::string& host, int pub_port)
  : host_{util::to_host_addr(host)}
  , pub_port_{pub_port}
  , sub_port_{default_sub_port(pub_port)}
{
    if (invalid_port(pub_port_)) {
        throw std::invalid_argument{"invalid pub port: " + std::to_string(pub_port_)};
    };
    if (invalid_port(sub_port_)) {
        throw std::invalid_argument{"invalid sub port: " + std::to_string(sub_port_)};
    };
}


RegAddress::RegAddress()
  : RegAddress{util::localhost(), default_port}
{ }


RegAddress::RegAddress(const std::string& host)
  : RegAddress{host, default_port}
{ }


RegAddress::RegAddress(const std::string& host, int port)
  : host_{util::to_host_addr(host)}
  , port_{port}
{
    if (invalid_port(port_)) {
        throw std::invalid_argument{"invalid reg port: " + std::to_string(port_)};
    };
}


std::ostream& operator<<(std::ostream& os, const ProxyAddress& a)
{
    return os << a.host() << ":" << a.pub_port();
}

std::ostream& operator<<(std::ostream& os, const RegAddress& a)
{
    return os << a.host() << ":" << a.port();
}

} // end namespace clara::msg
