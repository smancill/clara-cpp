/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_SERVICE_REPORT_HPP
#define CLARA_SERVICE_REPORT_HPP

#include "constants.hpp"

#include <atomic>
#include <cstdint>
#include <string>

namespace clara {

struct ServiceParameters
{
    std::string engine_name;
    std::string engine_lib;
    std::string initial_state;
    std::string description;
    int pool_size;
};


class ServiceReport
{
public:
    ServiceReport(std::string_view name,
                  const ServiceParameters& params,
                  std::string_view author,
                  std::string_view version,
                  std::string_view description);

public:
    auto name() const -> std::string_view { return name_; };

    auto engine() const -> std::string_view { return engine_; };

    auto library() const -> std::string_view { return library_; };

    auto author() const -> std::string_view { return author_; };

    auto lang() const -> std::string_view { return constants::cpp_lang; };

    auto pool_size() const -> int { return pool_size_; };

    auto description() const -> std::string_view { return description_; };

    auto version() const -> std::string_view { return version_; };

    auto start_time() const -> std::string_view { return start_time_; };

public:
    auto n_requests() const -> long { return n_requests_.load(); };

    auto n_failures() const -> long { return n_failures_.load(); };

    auto shm_reads() const -> long { return shm_reads_.load(); };

    auto shm_writes() const -> long { return shm_writes_.load(); };

    auto bytes_recv() const -> long { return bytes_recv_.load(); };

    auto bytes_sent() const -> long { return bytes_sent_.load(); };

    auto exec_time() const -> long { return exec_time_.load(); };

public:
    void add_n_requests() { n_requests_.fetch_add(1); };

    void add_n_failures() { n_failures_.fetch_add(1); };

    void add_shm_reads() { shm_reads_.fetch_add(1); };

    void add_shm_writes() { shm_writes_.fetch_add(1); };

    void add_bytes_recv(std::int64_t bytes) { bytes_recv_.fetch_add(bytes); };

    void add_bytes_sent(std::int64_t bytes) { bytes_sent_.fetch_add(bytes); };

    void add_exec_time(std::int64_t duration) { exec_time_.fetch_add(duration); };

private:
    std::string name_;
    std::string engine_;
    std::string library_;

    int pool_size_;
    std::string author_;
    std::string version_;
    std::string description_;
    std::string start_time_;

    std::atomic<std::int64_t> n_requests_{0};
    std::atomic<std::int64_t> n_failures_{0};
    std::atomic<std::int64_t> shm_reads_{0};
    std::atomic<std::int64_t> shm_writes_{0};
    std::atomic<std::int64_t> bytes_recv_{0};
    std::atomic<std::int64_t> bytes_sent_{0};
    std::atomic<std::int64_t> exec_time_{0};
};

} // end namespace clara

#endif // end of include guard: CLARA_SERVICE_REPORT_HPP
