/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "container_report.hpp"

#include "utils.hpp"

namespace clara {

ContainerReport::ContainerReport(std::string_view name,
                                 std::string_view author)
  : name_{name}
  , author_{author}
  , start_time_{util::get_current_time()}
{
    // nop
}


void ContainerReport::add_service(const element_type& service)
{
    services_.add(service);
}


void ContainerReport::remove_service(const element_type& service)
{
    services_.remove(service);
}


auto ContainerReport::services() const -> ContainerReport::range_type
{
    return services_.view();
}

}
