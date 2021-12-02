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

#include <clara/msg/topic.hpp>

#include <sstream>
#include <stdexcept>
#include <string>

namespace {

const char SEPARATOR = ':';

}


namespace clara::msg {

namespace detail {

auto get_domain(std::string_view topic) -> std::string_view
{
    auto firstSep = topic.find(SEPARATOR);
    if (firstSep == decltype(topic)::npos) {
        return topic;
    }
    return topic.substr(0, firstSep);
}


auto get_subject(std::string_view topic) -> std::string_view
{
    auto firstSep = topic.find(SEPARATOR);
    if (firstSep == decltype(topic)::npos) {
        return {Topic::ANY};
    }
    auto secondSep = topic.find(SEPARATOR, firstSep + 1);
    if (secondSep == decltype(topic)::npos) {
        return topic.substr(firstSep + 1);
    }
    return topic.substr(firstSep + 1, secondSep - firstSep - 1);
}


auto get_type(std::string_view topic) -> std::string_view
{
    auto firstSep = topic.find(SEPARATOR);
    if (firstSep == decltype(topic)::npos) {
        return {Topic::ANY};
    }
    auto secondSep = topic.find(SEPARATOR, firstSep + 1);
    if (secondSep == decltype(topic)::npos) {
        return {Topic::ANY};
    }
    return topic.substr(secondSep + 1);
}


auto is_parent(std::string_view topic, std::string_view other) -> bool
{
    return other.compare(0, topic.size(), topic) == 0;
}

} // end namespace detail


auto Topic::build(std::string_view domain,
                  std::string_view subject,
                  std::string_view type) -> Topic
{
    if (domain == ANY) {
        throw std::invalid_argument("domain is not defined");
    }
    auto topic = std::stringstream{};
    topic << domain;
    if (subject != ANY) {
        topic << SEPARATOR << subject;
        if (type != ANY) {
            auto ss = std::stringstream{};
            ss << type;
            auto tst = std::string{};
            while (std::getline(ss, tst, SEPARATOR)) {
                if (tst != ANY) {
                    topic << SEPARATOR << tst;
                } else {
                    break;
                }
            }
        }
    } else {
        if (type != ANY) {
            throw std::invalid_argument("subject is not defined");
        }
    }


    return {topic.str()};
}

} // end namespace clara::msg
