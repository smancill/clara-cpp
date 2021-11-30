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

namespace vl { // variable lenght integers (not provided by clara-msg helpers)

using clara::msg::proto::Data;
using clara::msg::proto::detail::set_repeated;

template <typename T>
inline void set_value(Data& /*data*/, const T& /*value*/)
        { static_assert(sizeof(T) == 0, "Unsupported data type"); }

inline void set_value(Data& data, std::int32_t value)
        { data.set_vlsint32(value); }
inline void set_value(Data& data, std::int64_t value)
        { data.set_vlsint64(value); }

inline void set_value(Data& data, const std::vector<std::int32_t>& value)
        { set_repeated(data.mutable_vlsint32a(), value); }
inline void set_value(Data& data, const std::vector<std::int64_t>& value)
        { set_repeated(data.mutable_vlsint64a(), value); }

template <typename T>
inline auto get_value(const Data& /*data*/) -> T
        { static_assert(sizeof(T) == 0, "Unsupported data type"); return T{}; }

template<> inline auto get_value(const Data& data) -> std::int32_t
        { return data.vlsint32(); }
template<> inline auto get_value(const Data& data) -> std::int64_t
        { return data.vlsint64(); }

template<> inline auto get_value(const Data& data) -> std::vector<std::int32_t>
        { const auto& a = data.vlsint32a(); return {a.begin(), a.end()}; }
template<> inline auto get_value(const Data& data) -> std::vector<std::int64_t>
        { const auto& a = data.vlsint64a(); return {a.begin(), a.end()}; }
}

// ---------------------------------------------------------------------------

template<typename T>
class PrimitiveSerializer : public clara::Serializer
{
public:
    auto write(const std::any& data) const -> std::vector<std::uint8_t> override
    {
        const T& value = std::any_cast<const T&>(data);
        const auto xdata = clara::msg::proto::make_data(value);
        return clara::msg::proto::serialize_data(xdata);
    }

    auto read(const std::vector<std::uint8_t>& buffer) const -> std::any override
    {
        const auto xdata = clara::msg::proto::parse_data(buffer);
        return {clara::msg::proto::parse_data<T>(xdata)};
    }
};


template<typename T>
class VariableLengthSerializer : public clara::Serializer
{
public:
    auto write(const std::any& data) const -> std::vector<std::uint8_t> override
    {
        const T& value = std::any_cast<const T&>(data);
        auto xdata = clara::msg::proto::Data{};
        vl::set_value(xdata, value);
        return clara::msg::proto::serialize_data(xdata);
    }

    auto read(const std::vector<std::uint8_t>& buffer) const -> std::any override
    {
        const auto xdata = clara::msg::proto::parse_data(buffer);
        return {vl::get_value<T>(xdata)};
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


class NativeSerializer : public clara::Serializer
{
public:
    auto write(const std::any& data) const -> std::vector<std::uint8_t> override
    {
        const auto& value = std::any_cast<const clara::msg::proto::Data&>(data);
        return clara::msg::proto::serialize_data(value);
    }

    auto read(const std::vector<std::uint8_t>& buffer) const -> std::any override
    {
        return {clara::msg::proto::parse_data(buffer)};
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

template<typename T>
auto s_vector() -> std::unique_ptr<clara::Serializer>
{
    return std::make_unique<PrimitiveSerializer<std::vector<T>>>();
}

template<typename T>
auto vl_primitive() -> std::unique_ptr<clara::Serializer>
{
    return std::make_unique<VariableLengthSerializer<T>>();
}

template<typename T>
auto vl_vector() -> std::unique_ptr<clara::Serializer>
{
    return std::make_unique<VariableLengthSerializer<std::vector<T>>>();
}

} // end namespace

// ---------------------------------------------------------------------------

namespace clara::type {

namespace mt = msg::mimetype;

const EngineDataType SINT32 { mt::single_sint32, vl_primitive<std::int32_t>() };
const EngineDataType SINT64 { mt::single_sint64, vl_primitive<std::int64_t>() };
const EngineDataType SFIXED32 { mt::single_sfixed32, s_primitive<std::int32_t>() };
const EngineDataType SFIXED64 { mt::single_sfixed64, s_primitive<std::int64_t>() };
const EngineDataType FLOAT { mt::single_float, s_primitive<float>() };
const EngineDataType DOUBLE { mt::single_double, s_primitive<double>() };
const EngineDataType STRING { mt::single_string, s_primitive<std::string>() };

const EngineDataType BYTES { "binary/bytes", std::make_unique<RawBytesSerializer>() };

const EngineDataType ARRAY_SINT32 { mt::array_sint32, vl_vector<std::int32_t>() };
const EngineDataType ARRAY_SINT64 { mt::array_sint64, vl_vector<std::int64_t>() };
const EngineDataType ARRAY_SFIXED32 { mt::array_sfixed32, s_vector<std::int32_t>() };
const EngineDataType ARRAY_SFIXED64 { mt::array_sfixed64, s_vector<std::int64_t>() };
const EngineDataType ARRAY_FLOAT { mt::array_float, s_vector<float>() };
const EngineDataType ARRAY_DOUBLE { mt::array_double, s_vector<double>() };
const EngineDataType ARRAY_STRING { mt::array_string, s_vector<std::string>() };

const EngineDataType JSON { "application/json", std::make_unique<StringSerializer>() };
const EngineDataType NATIVE { "native", std::make_unique<NativeSerializer>() };

} // end namespace clara::type
