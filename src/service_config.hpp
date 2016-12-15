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

    long data_count()
    {
        return data_req_count.load();
    }

    long done_count()
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

    long data_count_threshold()
    {
        return data_req_threshold.load();
    }

    long done_count_threshold()
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
