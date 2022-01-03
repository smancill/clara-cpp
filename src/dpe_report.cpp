/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dpe_report.hpp"

#include "base.hpp"
#include "json_utils.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <stdexcept>

static auto get_clara_home() -> std::string
{
    auto* clara_home = std::getenv("CLARA_HOME");  // NOLINT(concurrency-mt-unsafe): setenv is never used
    if (clara_home == nullptr) {
        throw std::runtime_error{"Missing CLARA_HOME env variable"};
    }
    return {clara_home};
}


static auto get_alive_report(std::string_view name,
                             int cores,
                             std::string_view clara_home) -> std::string
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


auto DpeReport::clara_home() const -> std::string_view
{
    return clara_home_;
}


auto DpeReport::core_count() const -> int
{
    return config_.max_cores;
}


auto DpeReport::memory_size() const -> long
{
    return 1;
}


auto DpeReport::memory_usage() const -> long
{
    return 1;
}


auto DpeReport::cpu_usage() const -> double
{
    return 1.0;
}


auto DpeReport::load() const -> double
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


auto DpeReport::containers() const -> DpeReport::range_type
{
    return containers_.view();
}

} // end namespace clara
