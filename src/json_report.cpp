/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "json_report.hpp"

#include "constants.hpp"
#include "container_report.hpp"
#include "dpe_report.hpp"
#include "json_utils.hpp"
#include "service_report.hpp"
#include "utils.hpp"

#include <string>
#include <string_view>


using namespace std::literals::string_view_literals;

static constexpr auto containers_key = "containers"sv;
static constexpr auto services_key = "services"sv;


namespace clara {

auto JsonReport::generate(const DpeReport& report) const -> std::string
{
    using namespace util;
    using namespace constants;

    auto buffer = Buffer{};
    auto writer = Writer{buffer};

    auto snapshot_time = util::get_current_time();

    writer.StartObject();

    writer.Key(runtime_key.data(), runtime_key.size());
    writer.StartObject();
    put(writer, "name", report.name());
    put(writer, "snapshot_time", snapshot_time);
    put(writer, "cpu_usage", report.cpu_usage());
    put(writer, "memory_usage", report.memory_usage());
    put(writer, "load", report.load());
    writer.Key(containers_key.data(), containers_key.size());
    writer.StartArray();
    for (const auto& cr : report.containers()) {
        auto cont_requests = 0L;
        writer.StartObject();
        put(writer, "name", cr->name());
        put(writer, "snapshot_time", snapshot_time);
        writer.Key(services_key.data(), services_key.size());
        writer.StartArray();
        for (const auto& sr : cr->services()) {
            auto service_requests = sr->n_requests();
            cont_requests += service_requests;
            writer.StartObject();
            put(writer, "name", sr->name());
            put(writer, "snapshot_time", snapshot_time);
            put(writer, "n_requests", service_requests);
            put(writer, "n_failures", sr->n_failures());
            put(writer, "shm_reads", sr->shm_reads());
            put(writer, "shm_writes", sr->shm_writes());
            put(writer, "bytes_recv", sr->bytes_recv());
            put(writer, "bytes_sent", sr->bytes_sent());
            put(writer, "exec_time", sr->exec_time());
            writer.EndObject();
        }
        writer.EndArray();
        put(writer, "n_requests", cont_requests);
        writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();

    writer.Key(registration_key.data(), registration_key.size());
    writer.StartObject();
    put(writer, "name", report.name());
    put(writer, "language", report.lang());
    put(writer, "clara_home", report.clara_home());
    put(writer, "n_cores", report.core_count());
    put(writer, "memory_size", report.memory_size());
    put(writer, "start_time", report.start_time());
    writer.Key(containers_key.data(), containers_key.size());
    writer.StartArray();
    for (const auto& cr : report.containers()) {
        writer.StartObject();
        put(writer, "name", cr->name());
        put(writer, "language", cr->lang());
        put(writer, "author", cr->author());
        put(writer, "start_time", cr->start_time());
        writer.Key(services_key.data(), services_key.size());
        writer.StartArray();
        for (const auto& sr : cr->services()) {
            writer.StartObject();
            put(writer, "name", sr->name());
            put(writer, "class_name", sr->library());
            put(writer, "author", sr->author());
            put(writer, "version", sr->version());
            put(writer, "description", sr->description());
            put(writer, "language", sr->lang());
            put(writer, "pool_size", sr->pool_size());
            put(writer, "start_time", sr->start_time());
            writer.EndObject();
        }
        writer.EndArray();
        writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();

    writer.EndObject();

    return buffer.GetString();
}

} // end namespace clara
