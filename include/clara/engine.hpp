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

#ifndef CLARA_ENGINE_HPP
#define CLARA_ENGINE_HPP

#include <clara/engine_data.hpp>
#include <clara/engine_data_type.hpp>

#include <set>
#include <string>
#include <vector>

/**
 * Core CLARA classes and functions.
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
