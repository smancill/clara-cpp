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

#include "composition.hpp"

#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace clara {
namespace composition {

//    Simple Compiler

SimpleCompiler::SimpleCompiler(std::string service_name)
  : service_name_{std::move(service_name)}
{
    // nop
}

void SimpleCompiler::compile(const std::string& composition)
{
    prev_.clear();
    next_.clear();

    auto sub_composition = &prev_;
    bool service_found = false;

    std::stringstream ss{composition};
    std::string service{};
    while (std::getline(ss, service, '+')) {
        if (service.back() == ';') {
            service.pop_back();
        }
        if (service == service_name_) {
            sub_composition = &next_;
            service_found = true;
            continue;
        }
        sub_composition->push_back(service);
    }
    if (!service_found) {
        throw std::logic_error{service_name_ + " not in: " + composition};
    }
}

std::set<std::string> SimpleCompiler::outputs()
{
    std::set<std::string> out;
    if (!next_.empty()) {
        out.insert(next_.front());
    }
    return out;
}


//    Composition Compiler

std::string IP = "([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})";

std::string WORD = "([A-Z|a-z]+[0-9]*)";
std::string PORT = "(%+[0-9]*)*";

std::string SERV_NAME = IP + PORT + "_(java|python|cpp)" + WORD + ":" + WORD;

std::string STATEMENT = SERV_NAME + "(," + SERV_NAME + ")*"
                        + "((+&?" + SERV_NAME + ")*|(+" + SERV_NAME
                        + "(," + SERV_NAME + ")*)*)";

std::regex SIMP_COND(SERV_NAME + "(==|!=)" + WORD + "\"");


CompositionCompiler::CompositionCompiler(std::string service)
        : my_service_name{std::move(service)}
{
    // nop
}

void CompositionCompiler::compile(std::string iCode)
{
    reset();

    std::string pCode = no_blanks(std::move(iCode));

    std::set<std::string> pp = pre_process(pCode);
}

void CompositionCompiler::reset()
{
    CompositionCompiler::instructions.clear();
}

std::set<instruction::Instruction> CompositionCompiler::get_instructions()
{
    //return instructions; error here, find out why
}

std::set<std::string> CompositionCompiler::get_unconditional_links()
{
    std::set<std::string> outputs;
//    for (instruction::Instruction inst : instructions) {
//        if (!inst.get_un_cond_statements().empty()) {
//            for (statement::Statement stmt : inst.get_un_cond_statements()) {
//                for (std::string s : stmt.get_output_links()) {
//                    outputs.insert(s);
//                }
//            }
//        }
//    }
    return outputs;
}

std::set<std::string> CompositionCompiler::get_links(ServiceState::ServiceState owner_ss,
                                                     ServiceState::ServiceState input_ss) {
    std::set<std::string> outputs;
    bool in_condition = false;
    bool condition_chosen = false;

//    for (instruction::Instruction inst : instructions) {
//        if (!inst.get_un_cond_statements().empty()) {
//            in_condition = false;
//            for (statement::Statement stmt : inst.get_un_cond_statements()) {
//                for (std::string s : stmt.get_output_links()) {
//                    outputs.insert(s);
//                }
//            }
//            continue;
//        }
//
//        if (!inst.get_if_condition().get_service_name().empty()) {
//            in_condition = true;
//            condition_chosen = false;
//            if (inst.get_if_condition().is_true(owner_ss, input_ss)) {
//                condition_chosen = true;
//                for (statement::Statement stmt : inst.get_if_cond_statements()) {
//                    for (std::string s : stmt.get_output_links()) {
//                        outputs.insert(s);
//                    }
//                }
//            }
//            continue;
//        }
//
//        if (in_condition && !condition_chosen) {
//            if (!inst.get_else_if_condition().get_service_name().empty()) {
//                if (inst.get_else_if_condition().is_true(owner_ss, input_ss)) {
//                    condition_chosen = true;
//                    for (statement::Statement stmt : inst.get_else_if_cond_statements()) {
//                        for (std::string s : stmt.get_output_links()) {
//                            outputs.insert(s);
//                        }
//                    }
//                }
//                continue;
//            }
//
//            if (!inst.get_else_cond_statements().empty()) {
//                condition_chosen = true;
//                for (statement::Statement stmt : inst.get_else_cond_statements()) {
//                    for (std::string s : stmt.get_output_links()) {
//                        outputs.insert(s);
//                    }
//                }
//            }
//        }
//    }
    return outputs;
}

std::set<std::string> CompositionCompiler::pre_process(std::string pCode)
{
    if (pCode.find(';') == std::string::npos && pCode.back() != ';') {
        throw std::logic_error{"Syntax error in the CLARA routing program. "
                                       "Missing end of statement operator = \";\""};
    }

    std::set<std::string> r;
    std::vector<std::string> st = tokenize(pCode, ";");

    for (std::string text : st) {
        if (text != "" && text != "}") {
            r.insert(text);
        }
    }

    return r;

}

bool CompositionCompiler::parse_statement(std::string iStmt)
{
    // todo
}

bool CompositionCompiler::parse_conditional_statement(std::string iStmt, instruction::Instruction ti)
{
    // todo
}

instruction::Instruction CompositionCompiler::parse_condition(std::string iCnd)
{
    // todo
}

std::string CompositionCompiler::no_blanks(std::string x)
{
    x.erase(std::remove (x.begin(), x.end(), ' '), x.end());
    return x;
}

} // end namespace composition
} // end namespace clara
