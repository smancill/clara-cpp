/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_CONSTANTS_HPP
#define CLARA_CONSTANTS_HPP

#include <string_view>

namespace clara::constants {

using namespace std::literals::string_view_literals;

// clang-format off
constexpr auto java_port = 7771;
constexpr auto cpp_port = 7781;
constexpr auto python_port = 7791;
constexpr auto reg_port_shift = 4;

constexpr auto java_lang = "java"sv;
constexpr auto python_lang = "python"sv;
constexpr auto cpp_lang = "cpp"sv;

constexpr auto dpe = "dpe"sv;
constexpr auto start_dpe = "startDpe"sv;
constexpr auto stop_dpe = "stopDpe"sv;
constexpr auto stop_remote_dpe = "stopRemoteDpe"sv;
constexpr auto dpe_exit = "dpeExit"sv;
constexpr auto ping_dpe = "pingDpe"sv;
constexpr auto ping_remote_dpe = "pingRemoteDpe"sv;
constexpr auto dpe_alive = "dpeAlive"sv;
constexpr auto dpe_report = "dpeReport"sv;

constexpr auto container = "container"sv;
constexpr auto state_container = "getContainerState"sv;
constexpr auto start_container = "startContainer"sv;
constexpr auto start_remote_container = "startRemoteContainer"sv;
constexpr auto stop_container = "stopContainer"sv;
constexpr auto stop_remote_container = "stopRemoteContainer"sv;
constexpr auto container_down = "containerIsDown"sv;
constexpr auto remove_container = "removeContainer"sv;
constexpr auto report_registration = "reportRegistration"sv;
constexpr auto report_runtime = "reportRuntime"sv;
constexpr auto report_json = "reportJson"sv;

constexpr auto registration_key = "DPERegistration"sv;
constexpr auto runtime_key = "DPERuntime"sv;

constexpr auto state_service = "getServiceState"sv;
constexpr auto start_service = "startService"sv;
constexpr auto start_remote_service = "startRemoteService"sv;
constexpr auto stop_service = "stopService"sv;
constexpr auto stop_remote_service = "stopRemoteService"sv;
constexpr auto deploy_service = "deployService"sv;
constexpr auto remove_service = "removeService"sv;
constexpr auto service_report_done = "serviceReportDone"sv;
constexpr auto service_report_data = "serviceReportData"sv;

constexpr auto set_front_end = "setFrontEnd"sv;
constexpr auto set_front_end_remote = "setFrontEndRemote"sv;

constexpr auto shared_memory_key = "clara/shmkey"sv;

constexpr auto mapkey_sep = "#"sv;
constexpr auto data_sep = "?"sv;
constexpr auto lang_sep = "_"sv;
constexpr auto port_sep = "%"sv;

constexpr auto info = "INFO"sv;
constexpr auto warning = "WARNING"sv;
constexpr auto error = "ERROR"sv;
constexpr auto done = "done"sv;
constexpr auto data = "data"sv;

constexpr auto udf = "undefined"sv;
// clang-format on

} // end namespace clara::constants

#endif // end of include guard: CLARA_CONSTANTS_HPP
