/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_DPE_REPORT_HPP
#define CLARA_DPE_REPORT_HPP

#include "concurrent_utils.hpp"
#include "constants.hpp"
#include "dpe_config.hpp"

#include <string>

namespace clara {

class Base;
class ContainerReport;

class DpeReport
{
public:
    using vector_type = util::CopyOnWriteVector<ContainerReport>;
    using element_type =  vector_type::pointer_type;
    using range_type = vector_type::range_type;

public:
    DpeReport(Base& base, DpeConfig& config);

public:
    auto name() const -> std::string_view { return name_; };

    auto session() const -> std::string_view { return config_.session; };

    auto lang() const -> std::string_view { return constants::cpp_lang; };

    auto description() const -> std::string_view { return config_.description; };

    auto start_time() const -> std::string_view { return start_time_; };

    auto clara_home() const -> std::string_view;

    auto alive_report() const -> std::string { return alive_report_; };

public:
    auto core_count() const -> int;

    auto memory_size() const -> long;

    auto memory_usage() const -> long;

    auto cpu_usage() const -> double;

    auto load() const -> double;

public:
    void add_container(const element_type& container);

    void remove_container(const element_type& container);

    auto containers() const -> range_type;

private:
    std::string name_;
    std::string start_time_;
    std::string alive_report_;
    std::string clara_home_;

    DpeConfig& config_;
    vector_type containers_;
};

} // end namespace clara

#endif // end of include guard: CLARA_DPE_REPORT_HPP
