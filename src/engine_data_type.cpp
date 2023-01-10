/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/engine_data_type.hpp>

#include <clara/msg/mimetype.hpp>
#include <clara/msg/proto/data.hpp>

#include <memory>

namespace {

template<typename T>
class PrimitiveSerializer : public clara::Serializer
{
public:
    auto write(const std::any& data) const -> std::vector<std::uint8_t> override
    {
        const T& value = std::any_cast<const T&>(data);
        return clara::msg::proto::detail::serialize_value(value);
    }

    auto read(const std::vector<std::uint8_t>& buffer) const -> std::any override
    {
        return clara::msg::proto::detail::parse_value<T>(buffer);
    }
};


class RawBytesSerializer : public clara::Serializer
{
public:
    auto write(const std::any& data) const -> std::vector<std::uint8_t> override
    {
        return std::any_cast<std::vector<std::uint8_t>>(data);
    }

    auto write(std::any&& data) const -> std::vector<std::uint8_t> override
    {
        return std::any_cast<std::vector<std::uint8_t>>(std::move(data));
    }

    auto read(const std::vector<std::uint8_t>& buffer) const -> std::any override
    {
        return {buffer};
    }

    auto read(std::vector<std::uint8_t>&& buffer) const -> std::any override
    {
        return {std::move(buffer)};
    }
};


class StringSerializer : public clara::Serializer
{
public:
    auto write(const std::any& data) const -> std::vector<std::uint8_t> override
    {
        const auto& value = std::any_cast<const std::string&>(data);
        return {std::begin(value), std::end(value)};
    }

    auto read(const std::vector<std::uint8_t>& buffer) const -> std::any override
    {
        return {std::string{std::begin(buffer), std::end(buffer)}};
    }
};

// ---------------------------------------------------------------------------

template<typename T>
auto s_primitive() -> std::unique_ptr<clara::Serializer>
{
    return std::make_unique<PrimitiveSerializer<T>>();
}

} // end namespace

// ---------------------------------------------------------------------------

namespace clara::type {

namespace mt = msg::mimetype;

const EngineDataType INT32 { mt::int32_number, s_primitive<std::int32_t>() };
const EngineDataType INT64 { mt::int64_number, s_primitive<std::int64_t>() };
const EngineDataType FLOAT { mt::float_number, s_primitive<float>() };
const EngineDataType DOUBLE { mt::double_number, s_primitive<double>() };
const EngineDataType STRING { mt::string, std::make_unique<StringSerializer>() };

const EngineDataType BYTES { "binary/bytes", std::make_unique<RawBytesSerializer>() };

const EngineDataType JSON { "application/json", std::make_unique<StringSerializer>() };

} // end namespace clara::type
