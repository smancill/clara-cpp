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

#ifndef CLARA_MSG_CONSTANTS_H_
#define CLARA_MSG_CONSTANTS_H_

#include <string>

namespace clara::msg::constants {

// clang-format off
inline const std::string success = "success";

inline const std::string register_publisher = "registerPublisher";
inline const std::string register_subscriber = "registerSubscriber";

inline const std::string remove_publisher = "removePublisherRegistration";
inline const std::string remove_subscriber = "removeSubscriberRegistration";
inline const std::string remove_all_registration = "removeAllRegistration";

inline const std::string find_publisher = "findPublisher";
inline const std::string find_subscriber = "findSubscriber";

constexpr auto register_request_timeout = 3000;
constexpr auto remove_request_timeout = 3000;
constexpr auto find_request_timeout = 3000;

inline const std::string ctrl_topic = "clara:msg:control";
inline const std::string ctrl_connect = "pub";
inline const std::string ctrl_subscribe = "sub";
inline const std::string ctrl_reply = "rep";
// clang-format on

} // end namespace clara::msg::constants

#endif // CLARA_MSG_CONSTANTS_H_
