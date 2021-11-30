/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_DPE_CONFIG_HPP
#define CLARA_DPE_CONFIG_HPP

#include <string>

namespace clara {

struct DpeConfig final
{
    static constexpr int default_pool_size = 2;
    static constexpr int default_report_period = 10'000;

    static const int default_max_cores;

    std::string session;
    std::string description;
    int pool_size = default_pool_size;
    int max_cores = default_max_cores;
    int report_period = default_report_period;
};

} // end namespace clara

#endif // end of include guard: CLARA_DPE_CONFIG_HPP
