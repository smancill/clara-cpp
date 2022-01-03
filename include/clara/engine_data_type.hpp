/*
 * Copyright (c) 2016.  Jefferson Lab (JLab). All rights reserved. Permission
 * to use, copy, modify, and distribute  this software and its documentation for
 * educational, research, and not-for-profit purposes, without fee and without a
 * signed licensing agreement.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY,
 * PROVIDED HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * This software was developed under the United States Government license.
 * For more information contact author at gurjyan@jlab.org
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 */

#ifndef CLARA_ENGINE_DATA_TYPE_HPP
#define CLARA_ENGINE_DATA_TYPE_HPP

#include <clara/serializer.hpp>

#include <memory>
#include <string>

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
    EngineDataType(std::string mime_type,
                   std::unique_ptr<Serializer> serializer)
      : mime_type_{std::move(mime_type)}
      , serializer_{std::move(serializer)}
    {
        // nothing
    }

    /**
     * Returns the name of this data type.
     */
    const std::string& mime_type() const
    {
        return mime_type_;
    }

    /**
     * Returns the serializer of this data type.
     */
    const Serializer* serializer() const
    {
        return serializer_.get();
    }

private:
    std::string mime_type_;
    std::shared_ptr<Serializer> serializer_;
};


inline
bool operator==(const EngineDataType& data_type, const std::string& mime_type)
{
    return data_type.mime_type() == mime_type;
}

inline
bool operator==(const std::string& mime_type, const EngineDataType& data_type)
{
    return data_type == mime_type;
}


inline
bool operator!=(const EngineDataType& data_type, const char* mime_type)
{
    return !(data_type == mime_type);
}

inline
bool operator!=(const std::string& mime_type, const EngineDataType& data_type)
{
    return !(data_type == mime_type);
}


/**
 * Predefined CLARA data types.
 */
namespace type {

/**
 * Signed int of 32 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType SINT32;

/**
 * Signed int of 64 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType SINT64;

/**
 * Signed fixed integer of 32 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType SFIXED32;

/**
 * Signed fixed integer of 64 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType SFIXED64;

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
 * An array of signed varints of 32 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType ARRAY_SINT32;

/**
 * An array of signed varints of 64 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType ARRAY_SINT64;

/**
 * An array of signed fixed integers of 32 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType ARRAY_SFIXED32;

/**
 * An array of signed fixed integers of 64 bits.
 *
 * @see <a href="https://developers.google.com/protocol-buffers/docs/encoding">Wire types</a>
 */
extern const EngineDataType ARRAY_SFIXED64;

/**
 * An array of floats (32 bits floating-point numbers).
 */
extern const EngineDataType ARRAY_FLOAT;

/**
 * An array of doubles (64 bits floating-point numbers).
 */
extern const EngineDataType ARRAY_DOUBLE;

/**
 * An array of strings.
 */
extern const EngineDataType ARRAY_STRING;

/**
 * JSON text.
 */
extern const EngineDataType JSON;

/**
 * A CLARA native data object.
 */
extern const EngineDataType NATIVE;

} // end namespace mime

} // end namespace clara

#endif // end of include guard: CLARA_ENGINE_DATA_TYPE_HPP
