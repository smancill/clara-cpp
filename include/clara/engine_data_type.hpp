/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_ENGINE_DATA_TYPE_HPP
#define CLARA_ENGINE_DATA_TYPE_HPP

#include <clara/serializer.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace clara {

/**
 * Defines a data type used by a {@link Engine service engine}.
 * Data type can be a predefined type, or a custom-defined type.
 * When declaring a custom type, its serialization routine must be provided.
 */
class EngineDataType final
{
public:
    /**
     * Creates a new user data type.
     * The data type is identified by its mime-type string.
     * The serializer will be used in order to send data through the network,
     * or to a different language DPE.
     *
     * @param mimeType the name of this data-type
     * @param serializer the custom serializer for this data-type
     */
    EngineDataType(std::string_view mime_type,
                   std::unique_ptr<Serializer> serializer)
      : mime_type_{mime_type}
      , serializer_{std::move(serializer)}
    {
        // nothing
    }

    /**
     * Returns the name of this data type.
     */
    auto mime_type() const -> const std::string&
    {
        return mime_type_;
    }

    /**
     * Returns the serializer of this data type.
     */
    auto serializer() const -> const Serializer*
    {
        return serializer_.get();
    }

private:
    std::string mime_type_;
    std::shared_ptr<Serializer> serializer_;
};


inline auto operator==(const EngineDataType& data_type,
                       std::string_view mime_type) -> bool
{
    return mime_type == data_type.mime_type();
}

inline auto operator==(std::string_view mime_type,
                       const EngineDataType& data_type) -> bool
{
    return mime_type == data_type.mime_type();
}


inline auto operator!=(const EngineDataType& data_type,
                       std::string_view mime_type) -> bool
{
    return mime_type != data_type.mime_type() ;
}

inline auto operator!=(std::string_view mime_type,
                       const EngineDataType& data_type) -> bool
{
    return mime_type != data_type.mime_type() ;
}


/**
 * Predefined Clara data types.
 */
namespace type {

/**
 * An integer of 32 bits.
 */
extern const EngineDataType INT32;

/**
 * An integer of 64 bits.
 */
extern const EngineDataType INT64;

/**
 * A float (32 bits floating-point number).
 */
extern const EngineDataType FLOAT;

/**
 * A double (64 bits floating-point number).
 */
extern const EngineDataType DOUBLE;

/**
 * A string.
 */
extern const EngineDataType STRING;

/**
 * Raw bytes.
 * On Java a {@link ByteBuffer} is used to wrap the byte array and its endianess.
 */
extern const EngineDataType BYTES;

/**
 * JSON text.
 */
extern const EngineDataType JSON;

/**
 * A Clara native data object.
 */
extern const EngineDataType NATIVE;

} // end namespace mime

} // end namespace clara

#endif // end of include guard: CLARA_ENGINE_DATA_TYPE_HPP
