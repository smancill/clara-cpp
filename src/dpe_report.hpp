/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set ts=8 sts=4 et sw=4 tw=80: */
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

#include "constants.hpp"
#include "dpe_config.hpp"

#include <string>

namespace clara {

class Base;

class DpeReport
{
public:
    DpeReport(Base& base, DpeConfig& config);

public:
    std::string name() const { return name_; };

    std::string session() const { return config_.session; };

    std::string lang() const { return constants::cpp_lang; };

    std::string description() const { return config_.description; };

    std::string start_time() const { return start_time_; };

    std::string clara_home() const;

    std::string alive_report() const { return alive_report_; };

public:
    int core_count() const;

    long memory_size() const;

    long memory_usage() const;

    double cpu_usage() const;

    double load() const;

private:
    std::string name_;
    std::string start_time_;
    std::string alive_report_;

    DpeConfig& config_;
};

} // end namespace clara

#endif // end of include guard: CLARA_DPE_REPORT_HPP
