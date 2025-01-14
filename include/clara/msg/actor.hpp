/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_ACTOR_H_
#define CLARA_MSG_ACTOR_H_

#include <clara/msg/address.hpp>
#include <clara/msg/connection.hpp>
#include <clara/msg/message.hpp>
#include <clara/msg/proto/registration.hpp>
#include <clara/msg/subscription.hpp>
#include <clara/msg/topic.hpp>

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <string_view>

/**
 * Core Clara classes and functions.
 */
namespace clara::msg {

class ConnectionSetup;

using CallbackFn = std::function<void(Message&)>;


/**
 * The main Clara pub/sub actor.
 *
 * Actors send messages to each other using pub/sub communications
 * through a cloud of proxies.
 * Registrar services provide registration and discoverability of actors.
 *
 * An actor has a _name_ for identification, a _default proxy_ intended for
 * long-term publication/subscription of messages, and a _default registrar_
 * where it can register and discover other long-term actors.
 * Unless otherwise specified, the local node and the standard ports will be
 * used for both default proxy and registrar.
 *
 * Publishers set specific _topics_ for their messages, and subscribers define
 * topics of interest to filter which messages they want to receive.
 * A _domain-specific callback_ defined by the subscriber will be executed every
 * time a message is received. This callback must be thread-safe,
 * and it can also be used to send responses or new messages.
 *
 * In order to publish or subscribe to messages, a _connection_ to a proxy must
 * be obtained. The actor owns and keeps a _pool of available connections_,
 * creating new ones as needed. When no address is specified, the _default
 * proxy_ will be used. The connections can be returned to the pool of available
 * connections, to avoid creating too many new connections. All connections will
 * be closed when the actor is destroyed.
 *
 * Multi-threaded publication of messages is fully supported, but every thread
 * must use its own connection. Subscriptions of messages always run in their
 * own background thread. It is recommended to always obtain and release the
 * necessary connections inside the thread that uses them. The _connect_ methods
 * will ensure that each thread gets a different connection.
 *
 * Publishers must be sending messages through the same _proxy_ than the
 * subscribers for the messages to be received. Normally, this proxy will be
 * the _default proxy_ of a long-term subscriber with many dynamic publishers, or
 * the _default proxy_ of a long-term publisher with many dynamic subscribers.
 * To have many publishers sending messages to many subscribers, they all must
 * _agree_ in the proxy. It is possible to use several proxies, but multiple
 * publications and subscriptions will be needed, and it may get complicated.
 *
 * Actors can register as publishers and/or subscribers with _registrar services_,
 * so other actors can discover them if they share the topic of interest.
 * Using the registration and discovery methods is always thread-safe.
 * The registrar service must be common to the actors, running in a known node.
 * If no address is specified, the _default registrar_ will be used.
 * Note that the registration will always set the _default proxy_ as the proxy
 * through which the actor is publishing/subscribed to messages.
 * If registration for different proxies is needed, multiple actors should be
 * used, each one with an appropriate default proxy.
 *
 * \see Message
 * \see Topic
 */
class Actor
{
public:
    /**
     * Creates an actor with default settings.
     * The local node and the standard ports will be used for both
     * default proxy and registrar.
     *
     * \param name the name of this actor
     */
    explicit Actor(std::string name);

    /**
     * Creates an actor specifying the default registrar to be used.
     * The local node and the standard ports will be used for the default proxy.
     *
     * \param name the name of this actor
     * \param default_registrar the address to the default registrar
     */
    explicit Actor(std::string name,
                   RegAddress default_registrar);

    /**
     * Creates an actor specifying the default proxy and registrar to be used.
     *
     * \param name the name of this actor
     * \param default_proxy the address to the default proxy
     * \param default_registrar the address to the default registrar
     */
    explicit Actor(std::string name,
                   ProxyAddress default_proxy,
                   RegAddress default_registrar);

    Actor(const Actor& rhs) = delete;
    Actor(Actor&& rhs) noexcept;

    auto operator=(const Actor& rhs) -> Actor& = delete;
    auto operator=(Actor&& rhs) noexcept -> Actor&;

    virtual ~Actor();

public:
    /**
     * Obtains a connection to the default proxy.
     * If there is no available connection, a new one will be created.
     */
    auto connect() -> ProxyConnection;

    /**
     * Obtains a connection to the specified proxy.
     * If there is no available connection, a new one will be created.
     *
     * \param addr the address of the proxy
     */
    auto connect(const ProxyAddress& addr) -> ProxyConnection;

    /**
     * Changes the setup of all created connections.
     * The new setup will be used for every new connection.
     *
     * \param setup the new default connection setup
     */
    void set_connection_setup(std::unique_ptr<ConnectionSetup> setup);

public:
    /**
     * Publishes a message through the specified proxy connection.
     *
     * \param connection the connection to the proxy
     * \param msg the message to be published
     */
    void publish(ProxyConnection& connection, Message& msg);

    /**
     * Publishes a message through the specified proxy connection and blocks
     * waiting for a response.
     *
     * The subscriber must publish the response to the topic given by the
     * `replyto` metadata field, through the same proxy.
     *
     * This method will throw if a response is not received before the timeout
     * expires.
     *
     * \param connection the connection to the proxy
     * \param msg the message to be published
     * \param timeout the length of time to wait a response, in milliseconds
     * \return the response message
     */
    auto sync_publish(ProxyConnection& connection,
                      Message& msg,
                      int timeout) -> Message;

    /**
     * Subscribes to a topic of interest through the specified proxy
     * connection.
     * A background thread will be started to receive the messages.
     *
     * \param topic the topic to select messages
     * \param connection the connection to the proxy
     * \param callback the user action to run when a message is received
     */
    auto subscribe(const Topic& topic,
                   ProxyConnection&& connection,
                   CallbackFn callback) -> std::unique_ptr<Subscription>;

    /**
     * Stops the given subscription.
     *
     * \param handler an active subscription
     */
    void unsubscribe(std::unique_ptr<Subscription> handler);

public:
    /**
     * Registers this actor as a publisher of the specified topic,
     * on the default registrar service.
     *
     * The actor will be registered as publishing through the default proxy.
     *
     * \param topic the topic to which messages will be published
     * \param description general description of the published messages
     */
    void register_as_publisher(const Topic& topic,
                               std::string_view description);

    /**
     * Registers this actor as a publisher of the specified topic,
     * on the given registrar service.
     *
     * The actor will be registered as publishing through the default proxy.
     *
     * \param addr the address to the registrar service
     * \param topic the topic to which messages will be published
     * \param description general description of the published messages
     */
    void register_as_publisher(const RegAddress& addr,
                               const Topic& topic,
                               std::string_view description);

    /**
     * Registers this actor as a subscriber of the specified topic,
     * on the default registrar service.
     *
     * The actor will be registered as subscribed through the default proxy.
     *
     * \param topic the topic of the subscription
     * \param description general description of the subscription
     */
    void register_as_subscriber(const Topic& topic,
                                std::string_view description);

    /**
     * Registers this actor as a subscriber of the specified topic,
     * on the given registrar service.
     *
     * The actor will be registered as subscribed through the default proxy.
     *
     * \param addr the address to the registrar service
     * \param topic the topic of the subscription
     * \param description general description of the subscription
     */
    void register_as_subscriber(const RegAddress& addr,
                                const Topic& topic,
                                std::string_view description);

    /**
     * Removes this actor as a publisher of the specified topic,
     * from the default registrar service.
     *
     * \param topic the topic to which messages were published
     */
    void deregister_as_publisher(const Topic& topic);

    /**
     * Removes this actor as a publisher of the specified topic,
     * from the given registrar service.
     *
     * \param addr the address to the registrar service
     * \param topic the topic to which messages were published
     */
    void deregister_as_publisher(const RegAddress& addr, const Topic& topic);

    /**
     * Removes this actor as a subscriber of the specified topic,
     * from the default registrar service.
     *
     * \param topic the topic of the subscription
     */
    void deregister_as_subscriber(const Topic& topic);

    /**
     * Removes this actor as a subscriber of the specified topic,
     * from the given registrar service.
     *
     * \param addr the address to the registrar service
     * \param topic the topic of the subscription
     */
    void deregister_as_subscriber(const RegAddress& addr, const Topic& topic);

    /**
     * Finds all publishers of the specified topic
     * that are registered on the default registrar service.
     *
     * \param topic the topic of interest
     * \return a set with the registration data of the matching publishers
     * \see Topic for the rules of matching topics
     */
    auto find_publishers(const Topic& topic) -> RegDataSet;

    /**
     * Finds all publishers of the specified topic
     * that are registered on the given registrar service.
     *
     * \param addr the address to the registrar service
     * \param topic the topic of interest
     * \return a set with the registration data of the matching publishers
     * \see Topic for the rules of matching topics
     */
    auto find_publishers(const RegAddress& addr, const Topic& topic) -> RegDataSet;

    /**
     * Finds all subscribers to the specified topic
     * that are registered on the default registrar service.
     *
     * \param topic the topic of interest
     * \return a set with the registration data of the matching subscribers
     * \see Topic for the rules of matching topics
     */
    auto find_subscribers(const Topic& topic) -> RegDataSet;

    /**
     * Finds all subscribers to the specified topic
     * that are registered on the given registrar service.
     *
     * \param addr the address to the registrar service
     * \param topic the topic of interest
     * \return a set with the registration data of the matching subscribers
     * \see Topic for the rules of matching topics
     */
    auto find_subscribers(const RegAddress& addr, const Topic& topic) -> RegDataSet;

public:
    /**
     * Returns the name of this actor
     */
    auto name() const -> const std::string&;

    /**
     * Returns the address of the default registrar service used by this actor
     */
    auto default_registrar() const -> const RegAddress&;

    /**
     * Returns the address of the default proxy used by this actor
     */
    auto default_proxy() const -> const ProxyAddress&;

private:
    struct Impl;
    std::unique_ptr<Impl> actor_;
};

} // end namespace clara::msg

#endif // CLARA_MSG_ACTOR_H_
