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

#ifndef CLARA_CONSTANTS_HPP
#define CLARA_CONSTANTS_HPP

#include <string>

namespace clara {

namespace constants {

// clang-format off
constexpr auto java_port = 7771;
constexpr auto cpp_port = 7781;
constexpr auto python_port = 7791;
constexpr auto reg_port_shift = 4;

inline const std::string java_lang = "java";
inline const std::string python_lang = "python";
inline const std::string cpp_lang = "cpp";

inline const std::string dpe = "dpe";
inline const std::string start_dpe = "startDpe";
inline const std::string stop_dpe = "stopDpe";
inline const std::string stop_remote_dpe = "stopRemoteDpe";
inline const std::string dpe_exit = "dpeExit";
inline const std::string ping_dpe = "pingDpe";
inline const std::string ping_remote_dpe = "pingRemoteDpe";
inline const std::string dpe_alive = "dpeAlive";
inline const std::string dpe_report = "dpeReport";

inline const std::string container = "container";
inline const std::string state_container = "getContainerState";
inline const std::string start_container = "startContainer";
inline const std::string start_remote_container = "startRemoteContainer";
inline const std::string stop_container = "stopContainer";
inline const std::string stop_remote_container = "stopRemoteContainer";
inline const std::string container_down = "containerIsDown";
inline const std::string remove_container = "removeContainer";
inline const std::string report_registration = "reportRegistration";
inline const std::string report_runtime = "reportRuntime";
inline const std::string report_json = "reportJson";

inline const std::string registration_key = "DPERegistration";
inline const std::string runtime_key = "DPERuntime";

inline const std::string state_service = "getServiceState";
inline const std::string start_service = "startService";
inline const std::string start_remote_service = "startRemoteService";
inline const std::string stop_service = "stopService";
inline const std::string stop_remote_service = "stopRemoteService";
inline const std::string deploy_service = "deployService";
inline const std::string remove_service = "removeService";
inline const std::string service_report_done = "serviceReportDone";
inline const std::string service_report_data = "serviceReportData";

inline const std::string set_front_end = "setFrontEnd";
inline const std::string set_front_end_remote = "setFrontEndRemote";

inline const std::string shared_memory_key = "clara/shmkey";

inline const std::string mapkey_sep = "#";
inline const std::string data_sep = "?";
inline const std::string lang_sep = "_";
inline const std::string port_sep = "%";

inline const std::string info = "INFO";
inline const std::string warning = "WARNING";
inline const std::string error = "ERROR";
inline const std::string done = "done";
inline const std::string data = "data";

inline const std::string udf = "undefined";
// clang-format on

} // end namespace constants

} // end namespace clara

#endif // end of include guard: CLARA_CONSTANTS_HPP
