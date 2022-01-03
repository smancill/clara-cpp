/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_ENGINE_DATA_HPP
#define CLARA_ENGINE_DATA_HPP

#include <clara/engine_status.hpp>

#include <any>
#include <memory>
#include <string>
#include <type_traits>

namespace clara {

namespace msg::proto {
class Meta;
}

class EngineDataType;

class EngineData final
{
public:
    EngineData();

    EngineData(const EngineData& rhs);
    EngineData(EngineData&& rhs) noexcept;

    auto operator=(const EngineData& rhs) -> EngineData&;
    auto operator=(EngineData&& rhs) noexcept -> EngineData&;

    ~EngineData();

public:
    auto mime_type() const -> const std::string&;

    auto data() const -> const std::any&
    {
        return data_;
    }

    auto data() -> std::any&
    {
        return data_;
    }

    template<typename S, typename T>
    void set_data(S&& type, T&& data)
    {
        set_mime_type(std::forward<S>(type));
        if constexpr (std::is_constructible_v<std::string, T> &&
                      not std::is_same_v<std::string, std::decay_t<T>>) {
            data_ = std::string{std::forward<T>(data)};
        } else {
            data_ = std::forward<T>(data);
        }
    }

    auto has_data() -> bool
    {
        return data_.has_value();
    }

private:
    void set_mime_type(const std::string& mime_type);
    void set_mime_type(std::string&& mime_type);
    void set_mime_type(const EngineDataType& data_type);

public:
    auto description() const -> const std::string&;

    void set_description(const std::string& description);
    void set_description(std::string&& description);

    auto status() const -> EngineStatus;

    auto status_severity() const -> int;

    void set_status(EngineStatus status, int severity = 1);

public:
    auto engine_state() const -> const std::string&;

    void set_engine_state(const std::string& state);
    void set_engine_state(std::string&& state);

    auto engine_name() const -> const std::string&;

    auto engine_version() const -> const std::string&;

public:
    auto communication_id() const -> long;

    void set_communication_id(long id);

    auto composition() const -> const std::string&;

    auto execution_time() const -> long;

private:
    friend class EngineDataAccessor;
    using Meta = msg::proto::Meta;

    EngineData(std::any&& data, std::unique_ptr<Meta>&& meta);

    std::any data_;
    std::unique_ptr<Meta> meta_;
};


template<typename T>
auto data_cast(EngineData& data) -> T&
{
    using V = std::add_lvalue_reference_t<T>;
    return std::any_cast<V>(data.data());
}


template<typename T>
auto data_cast(const EngineData& data) -> const T&
{
    using V = std::add_lvalue_reference_t<std::add_const_t<T>>;
    return std::any_cast<V>(data.data());
}


} // end namespace clara

#endif // end of include guard: CLARA_ENGINE_DATA_HPP
