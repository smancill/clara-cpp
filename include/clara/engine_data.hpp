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

#ifndef CLARA_ENGINE_DATA_HPP
#define CLARA_ENGINE_DATA_HPP

#include <clara/any.hpp>
#include <clara/engine_status.hpp>

#include <memory>
#include <string>

namespace xmsg {
namespace proto {
class Meta;
} // end namespace proto
} // end namespace xmsg

namespace clara {

class EngineData final
{
public:
    EngineData();

    EngineData(const EngineData& rhs);
    EngineData& operator=(const EngineData& rhs);

    EngineData(EngineData&& rhs);
    EngineData& operator=(EngineData&& rhs);

    ~EngineData();

public:
    const std::string& mime_type() const;

    template<typename T>
    T& data()
    {
        T* value = any_cast<T>(&data_);
        if (value == nullptr) {
            throw bad_any_cast();
        }
        return *value;
    }

    template<typename T>
    const T& data() const
    {
        const T* value = any_cast<T>(&data_);
        if (value == nullptr) {
            throw bad_any_cast();
        }
        return *value;
    }

    template<typename S, typename T>
    void set_data(S&& mime_type, T&& data)
    {
        data_ = std::forward<T>(data);
        set_mime_type(std::forward<S>(mime_type));
    }

    template<typename S>
    void set_data(S&& mime_type, const char* data)
    {
        data_ = std::string{data};
        set_mime_type(std::forward<S>(mime_type));
    }

    bool has_data()
    {
        return !data_.empty();
    }

private:
    void set_mime_type(const std::string& mime_type);

public:
    const std::string& description() const;

    void set_description(const std::string& description);

    EngineStatus status() const;

    int status_severity() const;

    void set_status(EngineStatus status);

    void set_status(EngineStatus status, int severity);

public:
    const std::string& engine_state() const;

    void set_engine_state(const std::string& state);

    const std::string& engine_name() const;

    const std::string& engine_version() const;

public:
    long communication_id() const;

    void set_communication_id(long id);

    const std::string& composition() const;

    long execution_time() const;

private:
    friend class EngineDataAccessor;
    using Meta = xmsg::proto::Meta;

    EngineData(any&& data, std::unique_ptr<Meta>&& meta);

    any data_;
    std::unique_ptr<Meta> meta_;
};

} // end namespace clara

#endif // end of include guard: CLARA_ENGINE_DATA_HPP
