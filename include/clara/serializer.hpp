/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_DATA_SERIALIZATION_H
#define CLARA_DATA_SERIALIZATION_H

#include <any>
#include <cstdint>
#include <vector>

namespace clara {

class Serializer
{
public:
    /**
     * Serializes the user object into a byte buffer and returns it.
     *
     * @param data the user object stored on the {@link EngineData}
     * @throws ClaraException if the data could not be serialized
     */
    virtual auto write(const std::any& data) const -> std::vector<std::uint8_t> = 0;

    /**
     * De-serializes the byte buffer into the user object and returns it.
     *
     * @param buffer the serialized data
     * @throws ClaraException if the data could not be deserialized
     */
    virtual auto read(const std::vector<std::uint8_t>& buffer) const -> std::any = 0;

public:
    /**
     * Serializes the user object into a byte buffer and returns it.
     *
     * @param data the user object stored on the {@link EngineData}
     * @throws ClaraException if the data could not be serialized
     */
    virtual auto read(std::vector<std::uint8_t>&& buffer) const -> std::any
    {
        return read(buffer);
    }

    /**
     * De-serializes the byte buffer into the user object and returns it.
     *
     * @param buffer the serialized data
     * @throws ClaraException if the data could not be deserialized
     */
    virtual auto write(std::any&& data) const -> std::vector<std::uint8_t>
    {
        return write(data);
    }

public:
    virtual ~Serializer() = default;
};

} // end namespace clara

#endif // end of include guard: CLARA_DATA_SERIALIZATION_H
