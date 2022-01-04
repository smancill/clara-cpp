/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_PROTO_REG_H_
#define CLARA_MSG_PROTO_REG_H_

#include "registration.pb.h"

#include <clara/msg/address.hpp>
#include <clara/msg/topic.hpp>

#include <set>

namespace clara::msg {

namespace proto {

/**
 * \class Registration
 * \brief Information of a registered pub or sub actor.
 *
 * Objects of this class describe a registered pub or sub Clara actor.
 * The `name` identifies an actor that is publishing or subscribed to
 * (`ownertype`) messages
 * of the specified topic (`domain`, `subject` and `type`)
 * on the proxy address (`host` and `port`).
 */

/// \cond HIDDEN_SYMBOLS
struct CompareRegistration
{
    auto operator()(const Registration& lhs, const Registration& rhs) const -> bool;
};
/// \endcond


/**
 * Gets the topic from the given %Registration data.
 */
inline auto parse_topic(const Registration& reg) -> Topic
{
    return Topic::build(reg.domain(), reg.subject(), reg.type());
}

/**
 * Gets the address from the given %Registration data.
 */
inline auto parse_address(const Registration& reg) -> ProxyAddress
{
    return {reg.host(), reg.port()};
}

auto operator==(const Registration& lhs, const Registration& rhs) -> bool;
auto operator!=(const Registration& lhs, const Registration& rhs) -> bool;

} // end namespace proto

/**
 * The set of Registration objects that result of a discovery query.
 */
using RegDataSet = std::set<proto::Registration, proto::CompareRegistration>;

} // end namespace clara::msg

#endif // CLARA_MSG_PROTO_REG_H_
