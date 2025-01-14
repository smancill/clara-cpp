/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_MESSAGE_HPP_
#define CLARA_MSG_MESSAGE_HPP_

#include <clara/msg/proto/data.hpp>
#include <clara/msg/proto/meta.hpp>
#include <clara/msg/topic.hpp>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <tuple>
#include <vector>


namespace clara::msg {

class Actor;

/**
 * The standard message for Clara pub/sub communications.
 *
 * Messages are composed of a topic, metadata and data.
 *
 * Clara actors send (receive) messages to (from) other actors, using the
 * **topic** to identify and filter messages of interest.
 * See the Topic class documentation for details about topic matching.
 *
 * The **data** is always a byte buffer containing the serialized representation
 * of the value or object to be sent. For simple data types, the
 * \ref make_message helper should be preferred to create messages.
 * Otherwise it is up to the client to define the serialization of custom
 * complex objects.
 * The created message will give access to the binary buffer, which should
 * be deserialized to get the data back. With simple types,
 * \ref parse_message should be sufficient.
 *
 * The **metadata** can be used to provide further description of the data.
 * The `datatype` field is mandatory to identify the type of the data,
 * and can be used by clients to check if the message contains data they can
 * work with. It is responsibility of the publisher to create messages with
 * proper data and datatype (i.e. the datatype matches the data).
 * See proto::Meta for all available fields.
 *
 * When an actor publishes a message and expects a response
 * (using Actor::sync_publish), the message will have the metadata field
 * `replyto` set to the topic where the response should be published.
 * To reuse the message for the response, construct it with \ref make_response.
 * Otherwise, create a new message using the \ref Message::replyto "replyto"
 * as the topic.
 */
class Message final
{
public:
    /**
     * Creates a new message with the given topic, metadata and serialized
     * data.
     *
     * The metadata must contain the correct data type identifier for the data,
     * and any other field required by the client.
     *
     * \tparam T Topic
     * \tparam V `std::vector<std::uint8_t>`
     * \param topic the topic of the message
     * \param metadata description of the data
     * \param data serialized user data
     */
    template<typename T, typename V>
    Message(T&& topic, std::unique_ptr<proto::Meta>&& metadata, V&& data)
      : topic_{std::forward<T>(topic)}
      , meta_{metadata ? std::move(metadata)
                       : throw std::invalid_argument{"null metadata"} }
      , data_{std::forward<V>(data)}
    { }

    /**
     * Creates a new message with the given topic, data type and serialized
     * data.
     *
     * The data type must be the correct identifier for the data.
     * The metadata will be automatically created with only the data type set.
     *
     * \tparam T Topic
     * \tparam S `std::string` or `const char*`
     * \tparam V `std::vector<std::uint8_t>`
     * \param topic the topic of the message
     * \param mimetype the (literal) string identifier of the data
     * \param data serialized user data
     */
    template<typename T, typename S, typename V,
             typename = std::enable_if_t<std::is_constructible_v<std::string, S>>>
    Message(T&& topic, S&& mimetype, V&& data)
      : topic_{std::forward<T>(topic)}
      , meta_{proto::make_meta()}
      , data_{std::forward<V>(data)}
    {
        static_assert(not std::is_null_pointer_v<std::remove_reference_t<S>>);
        proto::detail::set_datatype(*meta_, std::forward<S>(mimetype));
    }

    Message(const Message& other)
      : topic_{other.topic_}
      , meta_{proto::copy_meta(*other.meta_)}
      , data_{other.data_}
    { }

    Message(Message&&) = default;

    auto operator=(const Message& other) -> Message&
    {
        if (this != &other) {
            topic_ = other.topic_;
            meta_ = proto::copy_meta(*other.meta_);
            data_ = other.data_;
        }
        return *this;
    }

    auto operator=(Message&&) -> Message& = default;

    ~Message() = default;

public:
    friend void swap(Message& lhs, Message& rhs)
    {
        using std::swap;
        swap(lhs.topic_, rhs.topic_);
        swap(lhs.meta_, rhs.meta_);
        swap(lhs.data_, rhs.data_);
    }

public:
    /// Read-only access to the topic
    auto topic() const -> const Topic& { return topic_; }

    /// Read-only access to the metadata
    auto meta() const -> const proto::Meta* { return meta_.get(); }

    /// Read-only access to the serialized data
    auto data() const -> const std::vector<std::uint8_t>& { return data_; }

public:
    /// Gets the `datatype` identifier from the metadata.
    auto datatype() const -> const std::string& { return meta_->datatype(); }

    /// Checks if the metadata contains a `replyto` value
    auto has_replyto() const -> bool { return meta_->has_replyto(); }

    /// Gets a %Topic using the `replyto` value from the metadata.
    /// Useful when creating a response message.
    auto replyto() const -> Topic { return Topic::raw(meta_->replyto()); }

public:
    friend auto make_response(Message&& msg) -> Message;
    friend auto make_response(const Message& msg) -> Message;

private:
    friend Actor;
    Topic topic_;
    std::unique_ptr<proto::Meta> meta_;
    std::vector<std::uint8_t> data_;
};


/**
 * Creates a simple message with a data value of type U.
 *
 * The meta field `datatype` will be set accordingly.
 * Use \ref parse_message(const Message&) "parse_message" to get the value back.
 *
 * \tparam T Topic
 * \tparam U a simple primitive or string type
 * \param topic the topic of the message
 * \param data the value to be set in the message
 */
template<typename T, typename U>
inline auto make_message(T&& topic, U&& data) -> Message
{
    return {std::forward<T>(topic),
            proto::detail::get_mimetype<std::decay_t<U>>(),
            proto::detail::serialize_value(std::forward<U>(data))};
}



/**
 * Deserializes a data of type U from the given message.
 *
 * \tparam U a simple primitive or string type
 * \param msg the message
 * \return the value of type U contained in the message
 */
template<typename U>
inline auto parse_message(const Message& msg) -> U
{
    return proto::detail::parse_value<U>(msg.data());
}


/**
 * Moves the given message into a response message.
 *
 * - The topic will be set to \ref Message::replyto "msg.replyto()"
 * - The metadata will be reused (and the `replyto` field will be cleared)
 * - The data will be reused
 */
inline auto make_response(Message&& msg) -> Message
{
    msg.topic_ = msg.replyto();
    msg.meta_->clear_replyto();
    return std::move(msg);
}


/**
 * Copies the given message into a response message.
 *
 * - The topic will be set to \ref Message::replyto "msg.replyto()"
 * - The metadata will be copied (and the `replyto` field will be cleared)
 * - The data will be copied
 */
inline auto make_response(const Message& msg) -> Message
{
    auto meta = proto::copy_meta(*msg.meta_);
    meta->clear_replyto();
    return {msg.replyto(), std::move(meta), msg.data_};
}


inline auto operator==(const Message& lhs, const Message& rhs) -> bool
{
    return std::tie(lhs.topic().str(), *lhs.meta(), lhs.data())
        == std::tie(rhs.topic().str(), *rhs.meta(), rhs.data());
}

inline auto operator!=(const Message& lhs, const Message& rhs) -> bool
{
    return !(lhs == rhs);
}

} // end namespace clara::msg

#endif // CLARA_MSG_MESSAGE_H_
