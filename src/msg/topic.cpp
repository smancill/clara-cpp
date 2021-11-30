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

auto get_domain(const std::string& topic) -> std::string
{
    auto firstSep = topic.find(SEPARATOR);
    if (firstSep == std::string::npos) {
        return topic;
    }
    return topic.substr(0, firstSep);
}


auto get_subject(const std::string& topic) -> std::string
{
    auto firstSep = topic.find(SEPARATOR);
    if (firstSep == std::string::npos) {
        return {Topic::ANY};
    }
    auto secondSep = topic.find(SEPARATOR, firstSep + 1);
    if (secondSep == std::string::npos) {
        return topic.substr(firstSep + 1);
    }
    return topic.substr(firstSep + 1, secondSep - firstSep - 1);
}


auto get_type(const std::string& topic) -> std::string
{
    auto firstSep = topic.find(SEPARATOR);
    if (firstSep == std::string::npos) {
        return {Topic::ANY};
    }
    auto secondSep = topic.find(SEPARATOR, firstSep + 1);
    if (secondSep == std::string::npos) {
        return {Topic::ANY};
    }
    return topic.substr(secondSep + 1);
}


auto is_parent(const std::string& topic, const std::string& other) -> bool
{
    return other.compare(0, topic.size(), topic) == 0;
}

} // end namespace detail


auto Topic::build(const std::string& domain,
                  const std::string& subject,
                  const std::string& type) -> Topic
{
    if (domain == ANY) {
        throw std::invalid_argument("domain is not defined");
    }
    auto topic = std::stringstream{};
    topic << domain;
    if (subject != ANY) {
        topic << SEPARATOR << subject;
        if (type != ANY) {
            auto ss = std::stringstream{type};
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
