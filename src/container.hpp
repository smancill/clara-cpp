/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_CONTAINER_HPP
#define CLARA_CONTAINER_HPP

#include "base.hpp"

#include "concurrent_map.hpp"
#include "container_report.hpp"
#include "service.hpp"

#include <memory>
#include <mutex>
#include <string>

namespace clara {

class Container : public Base
{
public:
    Container(const Component& self,
              const Component& frontend,
              std::string_view description);

    Container(const Container&) = delete;

    auto operator=(const Container&) -> Container& = delete;

    ~Container() override;

public:
    void start();

    void stop();

public:
    void add_service(const ServiceParameters& params);

    auto remove_service(const std::string& engine_name) -> bool;

    void remove_services();

public:
    auto report() const -> std::shared_ptr<ContainerReport>;

private:
    std::mutex mutex_;

    util::ConcurrentMap<std::string, Service> services_;
    std::shared_ptr<ContainerReport> report_;
    std::string description_;
    bool running_;
};

} // end namespace clara

#endif // end of include guard: CLARA_CONTAINER_HPP
