/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_PROTO_META_H_
#define CLARA_MSG_PROTO_META_H_

#include "meta.pb.h"

#include <memory>
#include <stdexcept>

/**
 * Protobuf data classes and helpers.
 * Clara uses [Protocol Buffers](https://developers.google.com/protocol-buffers)
 * for storing and sending metadata, simple data and registration information
 * between actors. The protobuf classes are defined in this namespace, along
 * with some helper functions that simplify creating objects of those classes
 * for common use cases.
 */
namespace clara::msg::proto {

/**
 * \class Meta
 * \brief Metadata for pub/sub communications.
 *
 * The fields in this protobuf class can be used to further describe
 * the message data in pub/sub communications.
 *
 * The most important is `datatype`, which identifies the type of the
 * serialized data. It is required for custom data objects, but it is set
 * automatically if the message is created with \ref make_message.
 *
 * Most other fields are optional or reserved.
 * `byteorder` must be set in case endiannes is important for serialization.
 * `communicationid` can be set to identify messages.
 * `replyto` is set by Clara to control sync communications.
 * The reserved fields like `status`, `composition`, `action` and `control`
 * should be set by higher-level layers.
 */

namespace detail {

template<typename S,
         typename = std::enable_if_t<std::is_constructible_v<std::string, S>>>
inline void set_datatype(Meta& meta, S&& datatype)
{
    if constexpr (std::is_same_v<std::decay_t<S>, std::string_view>) {
        meta.set_datatype(datatype.data(), datatype.size());
    } else {
        meta.set_datatype(std::forward<S>(datatype));
    }
}

} // end namespace detail


/**
 * Creates an smart pointer to an empty %Meta object.
 */
inline auto make_meta() -> std::unique_ptr<Meta>
{
    return std::make_unique<Meta>();
}

/**
 * Creates an smart pointer to a copy of the given %Meta object.
 */
inline auto copy_meta(const Meta& meta) -> std::unique_ptr<Meta>
{
    return std::make_unique<Meta>(meta);
}


inline auto operator==(const Meta& lhs, const Meta& rhs) -> bool
{
    return lhs.SerializeAsString() == rhs.SerializeAsString();
}

inline auto operator!=(const Meta& lhs, const Meta& rhs) -> bool
{
    return !(lhs == rhs);
}

} // end namespace clara::msg::proto

#endif // CLARA_MSG_PROTO_META_H_
