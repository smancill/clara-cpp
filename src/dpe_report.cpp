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

#include "dpe_report.hpp"

#include "base.hpp"
#include "utils.hpp"

#include <cstdlib>

static const std::string clara_home = std::getenv("CLARA_HOME");

namespace clara {

DpeReport::DpeReport(Base& base, DpeConfig& config)
  : name_{base.name()}
  , start_time_{util::get_current_time()}
  , config_{config}
{
    alive_report_ = name_ + constants::data_sep +
                    std::to_string(core_count()) + constants::data_sep +
                    clara_home();
}


std::string DpeReport::clara_home() const
{
    return ::clara_home;
}


int DpeReport::core_count() const
{
    return config_.max_cores;
}


long DpeReport::memory_size() const
{
    return 1;
}


long DpeReport::memory_usage() const
{
    return 1;
}


double DpeReport::cpu_usage() const
{
    return 1.0;
}


double DpeReport::load() const
{
    return 1.0;
}


void DpeReport::add_container(const element_type& container)
{
    containers_.add(container);
}


void DpeReport::remove_container(const element_type& container)
{
    containers_.remove(container);
}


DpeReport::range_type DpeReport::containers() const
{
    return containers_.view();
}

} // end namespace clara
