/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_ENGINE_HPP
#define CLARA_ENGINE_HPP

#include <clara/engine_data.hpp>
#include <clara/engine_data_type.hpp>

#include <set>
#include <string>
#include <vector>

/**
 * Core Clara classes and functions.
 */
namespace clara {

class Engine
{
public:
    virtual auto configure(EngineData&) -> EngineData = 0;

    virtual auto execute(EngineData&) -> EngineData = 0;

    virtual auto execute_group(const std::vector<EngineData>&) -> EngineData = 0;

public:
    virtual auto input_data_types() const -> std::vector<EngineDataType> = 0;

    virtual auto output_data_types() const -> std::vector<EngineDataType> = 0;

    virtual auto states() const -> std::set<std::string> { return {}; }

public:
    virtual auto name() const -> std::string = 0;

    virtual auto author() const -> std::string = 0;

    virtual auto description() const -> std::string = 0;

    virtual auto version() const -> std::string = 0;

public:
    virtual void reset() { };

    virtual ~Engine() = default;
};

} // end namespace clara

#endif // end of include guard: CLARA_ENGINE_HPP
