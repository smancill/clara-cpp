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

#ifndef CLARA_MSG_UTIL_H_
#define CLARA_MSG_UTIL_H_

#include <string>
#include <vector>

/**
 * A collection of auxiliary functions.
 */
namespace clara::msg::util {

/**
 * Returns the localhost IP.
 *
 * \throws std::system_error if an I/O error occurs.
 */
auto localhost() -> std::string;

/**
 * Returns the IP addresses of the local node.
 * Useful when the host can have multiple network cards, i.e. IP addresses.
 *
 * This method skip loop-back (127.xxx), ink-local (169.254.xxx),
 * multicast (224.xxx through 238.xxx) and
 * broadcast (255.255.255.255) addresses.
 *
 * \return vector of IP addresses
 * \throws std::system_error if an I/O error occurs.
 */
auto get_localhost_addrs() -> std::vector<std::string>;

/**
 * Updates the IP addresses of the local node.
 *
 * This method skip loop-back (127.xxx), ink-local (169.254.xxx),
 * multicast (224.xxx through 238.xxx) and
 * broadcast (255.255.255.255) addresses.
 *
 * \throws std::system_error if an I/O error occurs.
 */
void update_localhost_addrs();

/**
 * Returns the IP address of the specified host.
 *
 * \param hostname The name of the host (accepts "localhost")
 * \return dotted notation of the IP address
 * \throws std::system_error if the IP could not be obtained
 */
auto to_host_addr(const std::string& hostname) -> std::string;

/**
 * Checks if the host name is an IPv4 address.
 *
 * \param hostname Host name of the computing node.
 * \return true if host name has an IP form.
 */
auto is_ipaddr(std::string_view hostname) -> bool;

/**
 * Thread sleep wrapper.
 *
 * \param millis the length of time to sleep in milliseconds
 */
void sleep(long millis);

} // end namespace clara::msg::util

#endif // CLARA_MSG_UTIL_H_
