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

#ifndef CLARA_ENGINE_HPP
#define CLARA_ENGINE_HPP

#include <clara/engine_data.hpp>
#include <clara/engine_data_type.hpp>

#include <set>
#include <string>
#include <vector>
#include <memory>

namespace clara {

class Engine
{
public:
    virtual EngineData configure(EngineData&) = 0;

    virtual EngineData execute(EngineData&) = 0;

    virtual EngineData execute_group(const std::vector<EngineData>&) = 0;

public:
    virtual std::vector<EngineDataType> input_data_types() const = 0;

    virtual std::vector<EngineDataType> output_data_types() const = 0;

    virtual std::set<std::string> states() const { return {};  }

public:
    virtual std::string name() const = 0;

    virtual std::string author() const = 0;

    virtual std::string description() const = 0;

    virtual std::string version() const = 0;

public:
    virtual void reset() { };

    virtual ~Engine() { };
};

} // end namespace clara

#endif // end of include guard: CLARA_ENGINE_HPP
