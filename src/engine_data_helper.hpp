/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_ENGINE_DATA_HELPER_HPP
#define CLARA_ENGINE_DATA_HELPER_HPP

#include <clara/engine_data.hpp>
#include <clara/engine_data_type.hpp>
#include <clara/engine_status.hpp>

#include <clara/msg/message.hpp>

#include <memory>
#include <string>

namespace clara {

class EngineDataAccessor final
{
public:
    auto create(std::any&& data, std::unique_ptr<msg::proto::Meta>&& meta)
        -> EngineData
    {
        return EngineData{std::move(data), std::move(meta)};
    }

    auto view_meta(const EngineData& data) -> const msg::proto::Meta*
    {
        return data.meta_.get();
    }


    auto view_meta(EngineData& data) -> msg::proto::Meta*
    {
        return data.meta_.get();
    }

    auto move_meta(EngineData& data) -> std::unique_ptr<msg::proto::Meta>&
    {
        return data.meta_;
    }

    auto serialize(const EngineData& data,
                   const msg::Topic& topic,
                   const std::vector<EngineDataType>& data_types) -> msg::Message
    {
        using Msg = msg::Message;

        const auto& mime_type = data.meta_->datatype();
        for (auto&& dt : data_types) {
            if (dt.mime_type() == mime_type) {
                try {
                    auto bb = dt.serializer()->write(data.data_);
                    auto mm = msg::proto::copy_meta(*data.meta_);
                    return Msg{topic, std::move(mm), std::move(bb)};
                } catch (const std::exception& e) {
                    throw std::runtime_error{"could not serialize " + mime_type + ": " + e.what()};
                }
            }
        }
        if (mime_type == type::STRING.mime_type()) {
            auto bb = type::STRING.serializer()->write(data.data_);
            auto mm = msg::proto::copy_meta(*data.meta_);
            return Msg{topic, std::move(mm), std::move(bb)};
        }
        throw std::runtime_error{"unsupported output mime-type = " + mime_type};
    }


    auto deserialize(const msg::Message& msg,
                     const std::vector<EngineDataType>& data_types) -> EngineData
    {
        const auto* metadata = msg.meta();
        const auto& mime_type = metadata->datatype();
        for (auto&& dt : data_types) {
            if (dt.mime_type() == mime_type) {
                try {
                    auto user_data = dt.serializer()->read(msg.data());
                    auto user_meta = msg::proto::copy_meta(*metadata);
                    return create(std::move(user_data), std::move(user_meta));
                } catch (const std::exception& e) {
                    throw std::runtime_error{"could not deserialize " + mime_type + ": " + e.what()};
                }
            }
        }
        throw std::runtime_error{"unsupported input mime-type = " + mime_type};
    }
};

} // end namespace clara

#endif // end of include guard: CLARA_ENGINE_DATA_HELPER_HPP
