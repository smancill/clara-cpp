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

#ifndef CLARA_COMPOSITION_COMPILER_HPP
#define CLARA_COMPOSITION_COMPILER_HPP

#include <list>
#include <set>
#include <string>
#include <vector>
#include <regex>
#include "instruction.hpp"
#include "service_state.hpp"

namespace clara {
namespace composition {

inline std::vector<std::string> tokenize(std::string s, std::string delim)
{
    std:: vector<std::string> tv;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delim)) != std::string::npos) {
        token = s.substr(0, pos);
        tv.push_back(token);
        s.erase(0, pos + delim.length());
    }
    return tv;
}

class SimpleCompiler
{
public:
    SimpleCompiler(std::string service_name);

    void compile(const std::string& composition);

    std::set<std::string> outputs();

private:
    std::string service_name_;
    std::list<std::string> prev_;
    std::list<std::string> next_;
};

class CompositionCompiler
{
public:
    //CompositionCompiler();
    CompositionCompiler(std::string service);
    void compile(std::string iCode);
    void reset();
    std::vector<instruction::Instruction> get_instructions();
    std::vector<std::string> get_unconditional_links();
    std::vector<std::string> get_links(ServiceState::ServiceState owner_ss, ServiceState::ServiceState input_ss);
    static std::regex get_simp_cond();

private:
    std::string STR, STR2, Sn, RStmt, sCond, cCond, Cond;
    std::vector<instruction::Instruction> instructions;
    std::string my_service_name;
    std::vector<std::string> pre_process(std::string pCode);
    bool parse_statement(std::string iStmt);
    bool parse_conditional_statement(std::string iStmt, instruction::Instruction ti);
    instruction::Instruction parse_condition(std::string iCnd);
    std::string no_blanks(std::string x);

};

} // end namespace composition
} // end namespace clara

#endif // end of include guard: CLARA_COMPOSITION_COMPILER_HPP
