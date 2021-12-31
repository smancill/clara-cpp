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
#include "json_utils.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <stdexcept>

static std::string get_clara_home()
{
    auto clara_home = std::getenv("CLARA_HOME");
    if (!clara_home) {
        throw std::runtime_error{"Missing CLARA_HOME env variable"};
    }
    return {clara_home};
}


static std::string get_alive_report(const std::string& name,
                                    int cores,
                                    const std::string& clara_home)
{
    using namespace clara::util;

    auto buffer = Buffer{};
    auto writer = Writer{buffer};

    writer.StartObject();
    put(writer, "name", name);
    put(writer, "n_cores", cores);
    put(writer, "clara_home", clara_home);
    writer.EndObject();

    return buffer.GetString();
}


namespace clara {

DpeReport::DpeReport(Base& base, DpeConfig& config)
  : name_{base.name()}
  , start_time_{util::get_current_time()}
  , clara_home_{get_clara_home()}
  , config_{config}
{
    alive_report_ = get_alive_report(name(), core_count(), clara_home());
}


std::string DpeReport::clara_home() const
{
    return clara_home_;
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
