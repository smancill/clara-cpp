/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/engine_data.hpp>
#include <clara/engine_data_type.hpp>

#include <clara/msg/proto/meta.hpp>

#include <stdexcept>


namespace clara {

EngineData::EngineData()
  : data_{}  // NOLINT
  , meta_{std::make_unique<Meta>()}
{
    // nop
}


EngineData::EngineData(std::any&& data, std::unique_ptr<Meta>&& meta)
  : data_{std::move(data)}
  , meta_{std::move(meta)}
{
    // nop
}


EngineData::EngineData(const EngineData& rhs)
  : data_{rhs.data_}
  , meta_{msg::proto::copy_meta(*rhs.meta_)}
{
    // nop
}

EngineData::EngineData(EngineData&&) noexcept = default;


auto EngineData::operator=(const EngineData& rhs) -> EngineData&
{
    if (this != &rhs) {
        data_ = rhs.data_;
        meta_ = msg::proto::copy_meta(*rhs.meta_);
    }
    return *this;
}

auto EngineData::operator=(EngineData&&) noexcept -> EngineData& = default;

EngineData::~EngineData() = default;


auto EngineData::mime_type() const -> const std::string&
{
    return meta_->datatype();
}


void EngineData::set_mime_type(const std::string& mime_type)
{
    meta_->set_datatype(mime_type);
}


void EngineData::set_mime_type(std::string&& mime_type)
{
    meta_->set_datatype(std::move(mime_type));
}


void EngineData::set_mime_type(const EngineDataType& data_type)
{
    meta_->set_datatype(data_type.mime_type());
}


auto EngineData::description() const -> const std::string&
{
    return meta_->description();
}


void EngineData::set_description(const std::string& description)
{
    meta_->set_description(description);
}


void EngineData::set_description(std::string&& description)
{
    meta_->set_description(std::move(description));
}


auto EngineData::status() const -> EngineStatus
{
    auto status = meta_->status();
    switch (status) {
        case msg::proto::Meta::INFO:
            return EngineStatus::INFO;
        case msg::proto::Meta::WARNING:
            return EngineStatus::WARNING;
        case msg::proto::Meta::ERROR:
            return EngineStatus::ERROR;
        default:
            throw std::domain_error{"unexpected meta status"};
    }
}


auto EngineData::status_severity() const -> int
{
    return meta_->severityid();
}


void EngineData::set_status(EngineStatus status, int severity)
{
    switch (status) {
        case EngineStatus::INFO:
            meta_->set_status(msg::proto::Meta::INFO);
            break;
        case EngineStatus::WARNING:
            meta_->set_status(msg::proto::Meta::WARNING);
            break;
        case EngineStatus::ERROR:
            meta_->set_status(msg::proto::Meta::ERROR);
            break;
        default:
            throw std::domain_error{"unexpected engine status"};
    }
    meta_->set_severityid(severity);
}


auto EngineData::engine_state() const -> const std::string&
{
    return meta_->senderstate();
}


void EngineData::set_engine_state(const std::string& state)
{
    meta_->set_senderstate(state);
}


void EngineData::set_engine_state(std::string&& state)
{
    meta_->set_senderstate(std::move(state));
}


auto EngineData::engine_name() const -> const std::string&
{
    return meta_->author();
}


auto EngineData::engine_version() const -> const std::string&
{
    return meta_->version();
}


auto EngineData::communication_id() const -> long
{
    return meta_->communicationid();
}


void EngineData::set_communication_id(long id)
{
    meta_->set_communicationid(id);
}


auto EngineData::composition() const -> const std::string&
{
    return meta_->composition();
}


auto EngineData::execution_time() const -> long
{
    return meta_->executiontime();
}

} // end namespace clara
