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

#include <clara/msg/actor.hpp>

#include <clara/msg/connection_pool.hpp>
#include <clara/msg/utils.hpp>

#include "connection_driver.hpp"
#include "likely.hpp"
#include "registration_driver.hpp"
#ifdef __APPLE__
#include "thread_local.hpp"
#endif

#ifdef __APPLE__
using ConPool = clara::msg::ConnectionPool;
using tls = clara::msg::detail::ThreadLocal<ConPool>;
#endif

namespace {
#ifdef __APPLE__
auto main_pool = std::shared_ptr<ConPool>{tls::getThreadInstance()};
#endif
}

namespace clara::msg {

/// \cond HIDDEN_SYMBOLS
struct Actor::Impl
{
    Impl(std::string&& name, ProxyAddress&& proxy_addr, RegAddress&& reg_addr)
      : name{std::move(name)}
      , id{detail::encode_identity(proxy_addr.host(), this->name)}
      , default_proxy_addr{std::move(proxy_addr)}
      , default_reg_addr{std::move(reg_addr)}
    { }

    auto con_pool() -> ConnectionPool*
    {
#ifdef __APPLE__
        return tls::getThreadInstance();
#else
        static thread_local ConnectionPool pool{};
        return &pool;
#endif
    }

    std::string name;
    std::string id;
    ProxyAddress default_proxy_addr;
    RegAddress default_reg_addr;
};


class ScopedSubscription final
{
public:
    ScopedSubscription(detail::ProxyDriver& connection, Topic&& topic)
      : connection_{connection}
      , topic_{std::move(topic)}
      , poller_{connection_.sub_socket()}
    {
        connection_.subscribe(topic_);
        util::sleep(10);  // NOLINT
    }

    ScopedSubscription(const ScopedSubscription&) = delete;

    auto operator=(const ScopedSubscription&) -> ScopedSubscription& = delete;

    ~ScopedSubscription()
    {
        connection_.unsubscribe(topic_);
    }

    auto poll(int timeout) -> bool
    {
        return poller_.poll(timeout);
    }

private:
    detail::ProxyDriver& connection_;
    Topic topic_;
    detail::BasicPoller poller_;
};
/// \endcond


Actor::Actor(std::string name)
  : Actor{std::move(name), {}, {}}
{ }


Actor::Actor(std::string name,
             RegAddress default_registrar)
  : Actor{std::move(name), {}, std::move(default_registrar)}
{ }


Actor::Actor(std::string name,
             ProxyAddress default_proxy,
             RegAddress default_registrar)
  : actor_{new Impl{std::move(name),
                    std::move(default_proxy),
                    std::move(default_registrar)}}
{ }


Actor::Actor(Actor &&) noexcept = default;

auto Actor::operator=(Actor &&) noexcept -> Actor& = default;

Actor::~Actor() = default;


auto Actor::connect() -> ProxyConnection
{
    return actor_->con_pool()->get_connection(actor_->default_proxy_addr);
}


auto Actor::connect(const ProxyAddress& addr) -> ProxyConnection
{
    return actor_->con_pool()->get_connection(addr);
}


void Actor::set_connection_setup(std::unique_ptr<ConnectionSetup> setup)
{
    return actor_->con_pool()->set_default_setup(std::move(setup));
}


void Actor::publish(ProxyConnection& connection, Message& msg)
{
    connection->send(msg);
}


auto Actor::sync_publish(ProxyConnection& connection,
                         Message& msg,
                         int timeout) -> Message
{
    auto return_addr = detail::get_unique_replyto(actor_->id);
    msg.meta_->set_replyto(return_addr);

    ScopedSubscription sub{*connection, Topic::raw(std::move(return_addr))};
    connection->send(msg);

    const auto dt = 10;
    auto t = 0;
    while (t < timeout) {
        if (sub.poll(dt)) {
            auto raw_msg = connection->recv();
            if (CLARA_LIKELY(raw_msg.size() == 3)) {
                return detail::parse_message(raw_msg);
            }
        }
        t += dt;
    }

    throw std::runtime_error("error: no response for time_out = " +
                             std::to_string(timeout) + " milli sec.");
}


auto Actor::subscribe(const Topic& topic,
                      ProxyConnection&& connection,
                      CallbackFn callback) -> std::unique_ptr<Subscription>
{
    return std::unique_ptr<Subscription>{
            new Subscription{topic, connection.release(), std::move(callback)}
    };
}


void Actor::unsubscribe(std::unique_ptr<Subscription> handler)
{
    handler->stop();
}


void Actor::register_as_publisher(const Topic& topic,
                                  std::string_view description)
{
    register_as_publisher(actor_->default_reg_addr, topic, description);
}


void Actor::register_as_publisher(const RegAddress& addr,
                                  const Topic& topic,
                                  std::string_view description)
{
    auto driver = actor_->con_pool()->get_connection(addr);
    auto proxy = actor_->default_proxy_addr;
    auto data = registration::create(actor_->name, description,
                                     proxy.host(), proxy.pub_port(),
                                     topic, true);
    driver->add(data, true);
}


void Actor::register_as_subscriber(const Topic& topic,
                                   std::string_view description)
{
    register_as_subscriber(actor_->default_reg_addr, topic, description);
}


void Actor::register_as_subscriber(const RegAddress& addr,
                                   const Topic& topic,
                                   std::string_view description)
{
    auto driver = actor_->con_pool()->get_connection(addr);
    auto proxy = actor_->default_proxy_addr;
    auto data = registration::create(actor_->name, description,
                                     proxy.host(), proxy.sub_port(),
                                     topic, false);
    driver->add(data, false);
}


void Actor::deregister_as_publisher(const Topic& topic)
{
    deregister_as_publisher(actor_->default_reg_addr, topic);
}


void Actor::deregister_as_publisher(const RegAddress& addr, const Topic& topic)
{
    auto driver = actor_->con_pool()->get_connection(addr);
    auto proxy = actor_->default_proxy_addr;
    auto data = registration::create(actor_->name, "",
                                     proxy.host(), proxy.pub_port(),
                                     topic, true);
    driver->remove(data, true);
}



void Actor::deregister_as_subscriber(const Topic& topic)
{
    deregister_as_subscriber(actor_->default_reg_addr, topic);
}


void Actor::deregister_as_subscriber(const RegAddress& addr, const Topic& topic)
{
    auto driver = actor_->con_pool()->get_connection(addr);
    auto proxy = actor_->default_proxy_addr;
    auto data = registration::create(actor_->name, "",
                                     proxy.host(), proxy.sub_port(),
                                     topic, false);
    driver->remove(data, false);
}


auto Actor::find_publishers(const Topic& topic) -> RegDataSet
{
    return find_publishers(actor_->default_reg_addr, topic);
}


auto Actor::find_publishers(const RegAddress& addr, const Topic& topic) -> RegDataSet
{
    auto driver = actor_->con_pool()->get_connection(addr);
    auto proxy = actor_->default_proxy_addr;
    auto data = registration::create(actor_->name, "",
                                     proxy.host(), proxy.pub_port(),
                                     topic, true);
    return driver->find(data, true);
}


auto Actor::find_subscribers(const Topic& topic) -> RegDataSet
{
    return find_subscribers(actor_->default_reg_addr, topic);
}


auto Actor::find_subscribers(const RegAddress& addr, const Topic& topic) -> RegDataSet
{
    auto driver = actor_->con_pool()->get_connection(addr);
    auto proxy = actor_->default_proxy_addr;
    auto data = registration::create(actor_->name, "",
                                     proxy.host(), proxy.sub_port(),
                                     topic, false);
    return driver->find(data, false);
}


auto Actor::name() const -> const std::string&
{
    return actor_->name;
}


auto Actor::default_registrar() const -> const RegAddress&
{
    return actor_->default_reg_addr;
}


auto Actor::default_proxy() const -> const ProxyAddress&
{
    return actor_->default_proxy_addr;
}

} // end namespace clara::msg
