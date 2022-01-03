/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_JSON_REPORT_HPP
#define CLARA_JSON_REPORT_HPP

#include <string>

namespace clara {

class DpeReport;

class JsonReport
{
public:
    auto generate(const DpeReport& report) const -> std::string;
};

} // end namespace clara

#endif // end of include guard: CLARA_JSON_REPORT_HPP
