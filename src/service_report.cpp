/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "service_report.hpp"

#include "utils.hpp"

namespace clara {

ServiceReport::ServiceReport(std::string_view name,
                             const ServiceParameters& params,
                             std::string_view author,
                             std::string_view version,
                             std::string_view description)
  : name_{name}
  , engine_{params.engine_name}
  , library_{params.engine_lib}
  , pool_size_{params.pool_size}
  , author_{author}
  , version_{version}
  , description_{description}
  , start_time_{util::get_current_time()}
{
    // nop
}

}
