/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
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
