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

#ifndef CLARA_UTILS_HPP
#define CLARA_UTILS_HPP

#include <string>


namespace clara::msg::util {

std::string get_current_time();

}

namespace clara::util {

using msg::util::get_current_time;  // NOLINT

std::string get_dpe_name(const std::string& canonical_name);

std::string get_container_name(const std::string& canonical_name);

std::string get_container_canonical_name(const std::string& canonical_name);

std::string get_engine_name(const std::string& canonical_name);

std::string get_dpe_host(const std::string& canonical_name);

int get_dpe_port(const std::string& canonical_name);

std::string get_dpe_lang(const std::string& canonical_name);

int get_default_port(const std::string& lang);

} // end namespace clara::util

#endif // end of include guard: CLARA_UTILS_HPP
