/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_SERVICE_CONFIG_HPP
#define CLARA_SERVICE_CONFIG_HPP

#include "constants.hpp"

#include <atomic>
#include <string>

namespace clara {

class ServiceConfig
{
public:
    void add_request()
    {
        data_req_count.fetch_add(1);
        done_req_count.fetch_add(1);
    }

    void reset_data_count()
    {
        data_req_count.store(0);
    }

    void reset_done_count()
    {
        done_req_count.store(0);
    }

    auto data_count() -> long
    {
        return data_req_count.load();
    }

    auto done_count() -> long
    {
        return done_req_count.load();
    }

    void set_data_count_threshold(long limit)
    {
        data_req_threshold.store(limit);
    }

    void set_done_count_threshold(long limit)
    {
        done_req_threshold.store(limit);
    }

    auto data_count_threshold() -> long
    {
        return data_req_threshold.load();
    }

    auto done_count_threshold() -> long
    {
        return done_req_threshold.load();
    }

private:
    std::atomic<std::int64_t> data_req_threshold{0};
    std::atomic<std::int64_t> done_req_threshold{0};

    std::atomic<std::int64_t> data_req_count{0};
    std::atomic<std::int64_t> done_req_count{0};
};

} // end namespace clara

#endif // end of include guard: CLARA_SERVICE_CONFIG_HPP
