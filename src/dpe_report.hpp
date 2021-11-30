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
    auto name() const -> std::string { return name_; };

    auto session() const -> std::string { return config_.session; };

    auto lang() const -> std::string { return constants::cpp_lang; };

    auto description() const -> std::string { return config_.description; };

    auto start_time() const -> std::string { return start_time_; };

    auto clara_home() const -> std::string;

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
