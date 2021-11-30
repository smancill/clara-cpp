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
    auto create(any&& data, std::unique_ptr<msg::proto::Meta>&& meta) -> EngineData
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
