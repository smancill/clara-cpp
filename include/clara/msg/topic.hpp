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

#ifndef CLARA_MSG_TOPIC_H_
#define CLARA_MSG_TOPIC_H_

#include <string>
#include <utility>


namespace clara::msg {

namespace detail {

auto get_domain(const std::string& topic) -> std::string;
auto get_subject(const std::string& topic) -> std::string;
auto get_type(const std::string& topic) ->  std::string;
auto is_parent(const std::string& topic, const std::string& other) -> bool;

} // end namespace detail


/**
 * The standard identification for CLARA pub/sub communications.
 *
 * The CLARA messaging system is **topic-based**, and messages are published
 * to given "topics", or named channels, defined by _publishers_.
 * _Subscribers_ can received the messages published to the "topics" to
 * which they are interested, by subscribing to them.
 *
 * In the CLARA messaging system, a topic is composed of three parts: a
 * *domain*, a *subject* and a *type*. Each part is separated by a semicolon
 * character. The subject and the type can be omitted. Thus, the following
 * topics are all valid:
 *
 * - \c "domain"
 * - \c "domain:subject"
 * - \c "domain:subject:type"
 *
 * The \ref #build factory methods help to create a proper topic with the
 * right format.
 *
 * When a CLARA pub-sub actor is subscribed to a given topic, it will only
 * receive messages published to that topic. To filter topics, the three parts
 * form a hierarchy, and all topics with the same prefix will be accepted.
 *
 * In other words, a subscriber listening for an specific "domain" will receive
 * all messages whose topic starts with that domain, no matter the subject and
 * the type. For example, if the subscription topic is \c "A", then all the
 * messages with the following topics will be received:
 *
 * - \c "A"
 * - \c "A:B"
 * - \c "A:C"
 * - \c "A:B:1"
 * - \c "A:C:1"
 * - \c "A:C:2"
 * - etc...
 *
 * More specific subscriptions will not receive messages that match only the
 * parent parts of the topic.
 * Thus, subscription to \c "A:B" will accept \c "A:B", \c "A:B:1",
 * \c "A:B:2", etc, but will reject \c "A" or \c "A:C".
 * Similarly, a subscription to \c "A:B:1" will only accept that exact
 * topic, rejecting \c "A:B:2", \c "A:C", \c "A", etc.
 */
class Topic final
{
public:
    /**
     * Builds a new topic with only a domain part.
     *
     * \param domain the domain of the topic
     */
    static auto build(const std::string& domain) -> Topic
    {
        return build(domain, ANY, ANY);
    }

    /**
     * Builds a new topic with only domain and subject parts.
     *
     * \param domain the domain of the topic
     * \param subject the subject of the topic
     */
    static auto build(const std::string& domain,
                      const std::string& subject) -> Topic
    {
        return build(domain, subject, ANY);
    }

    /**
     * Builds a new full topic with domain, subject and type.
     *
     * \param domain the domain of the topic
     * \param subject the subject of the topic
     * \param type the type of the subject
     */
    static auto build(const std::string& domain,
                      const std::string& subject,
                      const std::string& type) -> Topic;

    /**
     * Use the given string as topic.
     * No validation is done to the string.
     * The caller must be sure it is a valid topic.
     * This factory method is provided for speed purposes.
     * It should be used with caution.
     *
     * \param topic a valid topic string
     */
    template<typename T,
             typename = std::enable_if_t<std::is_constructible_v<std::string, T>>>
    static auto raw(T&& topic) -> Topic
    {
        return {std::forward<T>(topic)};
    }

    static inline const std::string ANY = "*";

public:
    /**
     * Returns the domain part of the topic.
     */
    auto domain() const -> std::string
    {
        return detail::get_domain(topic_);
    }

    /**
     * Returns the subject part of the topic.
     * If the topic has no subject, then \c "*" is returned.
     */
    auto subject() const -> std::string
    {
        return detail::get_subject(topic_);
    }

    /**
     * Returns the type part of the topic.
     * If the topic has no type, then \c "*" is returned.
     */
    auto type() const -> std::string
    {
        return detail::get_type(topic_);
    }

    /**
     * Returns true if this topic is a parent of the given topic.
     * A parent topic is a prefix of other topic, or they are the same.
     * Examples:
     *
     * - \c "A" is a parent of \c "A:B" and \c "A:C:1"
     * - \c "A" is NOT parent of \c "W:B" nor \c "Z"
     * - \c "A:C" is a parent of \c "A:C:1" and \c "A:C"
     * - \c "A:C" is NOT a parent of \c "A:B"
     *
     * A subscription to a parent topic will accept any children topic.
     * See the class documentation for more details about filtering messages by
     * topic.
     *
     * \param other the topic to match as a children
     * \return true if this topic is a parent of the other
     */
    auto is_parent(const Topic& other) const -> bool
    {
        return detail::is_parent(topic_, other.topic_);
    }

    /**
     * Returns the string representation of this topic.
     */
    auto str() const -> const std::string&
    {
        return topic_;
    };

private:
    /**
     * Constructs a topic from the given string.
     * The string must be a valid topic.
     */
    template <
        typename T,
        typename = std::enable_if_t<
            !std::is_same<Topic, std::decay_t<T>>::value
        >
    >
    Topic(T&& topic)
      : topic_{std::forward<T>(topic)}
    { }

private:
    std::string topic_;
};


inline auto operator==(const Topic& lhs, const Topic& rhs) -> bool
{
    return lhs.str() == rhs.str();
}


inline auto operator!=(const Topic& lhs, const Topic& rhs) -> bool
{
    return !(lhs == rhs);
}

} // end namespace clara::msg

#endif // CLARA_MSG_TOPIC_H_
