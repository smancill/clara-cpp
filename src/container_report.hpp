/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_CONTAINER_REPORT_HPP
#define CLARA_CONTAINER_REPORT_HPP

#include "concurrent_utils.hpp"
#include "constants.hpp"

#include <string>

namespace clara {

class ServiceReport;

class ContainerReport
{
public:
    using vector_type = util::CopyOnWriteVector<ServiceReport>;
    using element_type =  vector_type::pointer_type;
    using range_type = vector_type::range_type;


public:
    ContainerReport(std::string_view name, std::string_view author);

public:
    auto name() -> std::string_view { return name_; };

    auto author() -> std::string_view { return author_; };

    auto lang() -> std::string_view { return constants::cpp_lang; };

    auto start_time() -> std::string_view { return start_time_; };

public:
    void add_service(const element_type& service);

    void remove_service(const element_type& service);

    auto services() const -> range_type;

private:
    std::string name_;
    std::string author_;
    std::string start_time_;

    vector_type services_;
};

} // end namespace clara

#endif // end of include guard: CLARA_CONTAINER_REPORT_HPP
