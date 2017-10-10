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

#include <iostream>
#include <string>
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

std::regex Statement_r(STATEMENT);


std::string simp_cond_s = SERV_NAME + "(==|!=)" + WORD + "\"";
std::regex SIMP_COND(simp_cond_s);

std::string comp_cond_s = simp_cond_s + "((&&|!!)" + simp_cond_s + ")*";
std::regex COMP_COND(comp_cond_s);

std::string cond_s = "((}?if|}elseif)(" + comp_cond_s + "){" + STATEMENT + ")|(}else{" + STATEMENT + ")";
std::regex COND(cond_s);

CompositionCompiler::CompositionCompiler(std::string service)
        : my_service_name{std::move(service)}
{
    // nop
}

std::regex CompositionCompiler::get_simp_cond() {
    return SIMP_COND;
}

// todo : implement rest of compile
void CompositionCompiler::compile(std::string iCode)
{
    reset();

    std::string pCode = no_blanks(std::move(iCode));

    std::vector<std::string> pp = pre_process(pCode);
    
}

void CompositionCompiler::reset()
{
    CompositionCompiler::instructions.clear();
}

std::vector<instruction::Instruction> CompositionCompiler::get_instructions()
{
    return instructions;
}

std::vector<std::string> CompositionCompiler::get_unconditional_links()
{
    std::vector<std::string> outputs;
    if (!instructions.empty()) {
        for(int i = 0; i < instructions.size(); i++) {
            if (!(instructions.at(i).get_un_cond_statements().empty())) {
                for (int j = 0; j < instructions.at(i).get_un_cond_statements().size(); j++) {
                    for (std::string s : instructions.at(i).get_un_cond_statements().at(j).get_output_links()) {
                        outputs.push_back(s);
                    }
                }
            }
        }
    }
    return outputs;
}

std::vector<std::string> CompositionCompiler::get_links(ServiceState::ServiceState owner_ss,
                                                     ServiceState::ServiceState input_ss) {
    std::vector<std::string> outputs;
    bool in_condition = false;
    bool condition_chosen = false;

    for (instruction::Instruction inst : instructions) {
        if (!inst.get_un_cond_statements().empty()) {
            in_condition = false;
            for (statement::Statement stmt : inst.get_un_cond_statements()) {
                for (std::string s : stmt.get_output_links()) {
                    outputs.push_back(s);
                }
            }
            continue;
        }

        if (!inst.get_if_condition().get_service_name().empty()) {
            in_condition = true;
            condition_chosen = false;
            if (inst.get_if_condition().is_true(owner_ss, input_ss)) {
                condition_chosen = true;
                for (statement::Statement stmt : inst.get_if_cond_statements()) {
                    for (std::string s : stmt.get_output_links()) {
                        outputs.push_back(s);
                    }
                }
            }
            continue;
        }

        if (in_condition && !condition_chosen) {
            if (!inst.get_else_if_condition().get_service_name().empty()) {
                if (inst.get_else_if_condition().is_true(owner_ss, input_ss)) {
                    condition_chosen = true;
                    for (statement::Statement stmt : inst.get_else_if_cond_statements()) {
                        for (std::string s : stmt.get_output_links()) {
                            outputs.push_back(s);
                        }
                    }
                }
                continue;
            }

            if (!inst.get_else_cond_statements().empty()) {
                condition_chosen = true;
                for (statement::Statement stmt : inst.get_else_cond_statements()) {
                    for (std::string s : stmt.get_output_links()) {
                        outputs.push_back(s);
                    }
                }
            }
        }
    }
    return outputs;
}

std::vector<std::string> CompositionCompiler::pre_process(std::string pCode)
{
    if (pCode.find(';') == std::string::npos && pCode.back() != ';') {
        throw std::logic_error{"Syntax error in the CLARA routing program. "
                                       "Missing end of statement operator = \";\""};
    }

    std::vector<std::string> r;
    std::vector<std::string> st = tokenize(pCode, ";");

    for (std::string text : st) {
        if (text != "" && text != "}") {
            r.push_back(text);
        }
    }

    return r;

}

bool CompositionCompiler::parse_statement(std::string iStmt)
{
    bool b = false;
    instruction::Instruction ti(iStmt);
    iStmt = remove_first(iStmt, '}');

    std::smatch matcher;

    try {
        if (std::regex_search(iStmt, matcher, Statement_r)) {  // todo : this might be wrong, test to check
            if (iStmt.find(my_service_name) == std::string::npos) {
                return false;
            }

            statement::Statement ts(iStmt, my_service_name);
            ti.add_un_cond_statement(ts);
            instructions.push_back(ti);
            b = true;
        } else {
            std::cout << "DDD ----- > statement = " + iStmt << std::endl;
            throw std::logic_error{"Syntax error in the CLARA routing program."
                                           "Malformed routing statement"};
        }
    } catch (const std::logic_error& e) {
        std::cout << e.what() << std::endl;
    }
    return b;
}

bool CompositionCompiler::parse_conditional_statement(std::string iStmt, instruction::Instruction ti)
{
    bool b = false;
    std::smatch matcher;

    if (std::regex_search(iStmt, matcher, Statement_r)) {
        if (iStmt.find(my_service_name) == std::string::npos) {
            return false;
        }

        statement::Statement ts(iStmt, my_service_name);

        // make a default instruction for comparison
        instruction::Instruction default_instruction("default_instruction");
        if (ti.get_if_condition().equals(default_instruction.get_if_condition())) {
            ti.add_if_cond_statement(ts);
        } else if (ti.get_else_if_condition().equals(default_instruction.get_else_if_condition())) {
            ti.add_else_if_cond_statement(ts);
        } else {
            ti.add_else_cond_statement(ts);
        }
        b = true;
    } else {
        std::cout << "DDD ----- > statement = " + iStmt << std::endl;
        throw std::logic_error{"Syntax error in the CLARA routing program."
                                       "Malformed routing statement"};
    }

    return b;
}

instruction::Instruction CompositionCompiler::parse_condition(std::string iCnd)
{
    instruction::Instruction ti(my_service_name);
    std::smatch matcher;

    if (std::regex_search(iCnd, matcher, COND)) {
        try {
            std::string statement_str = iCnd.substr(iCnd.find('{'));

            if (statement_str.find(my_service_name) == std::string::npos) {
                return ti; // todo : in Java this returns null, find replacement
            }

            statement::Statement ts(statement_str, my_service_name);

            if (std::strncmp(iCnd.c_str(), "}if(", 4) == 0 ||
                                            strncmp(iCnd.c_str(), "if(", 3) == 0) {
                std::string condition_str = iCnd.substr(iCnd.find('(') + 1,
                                                        iCnd.find_last_of(')'));
                condition::Condition tc(condition_str, my_service_name);
                ti.set_if_condition(tc);
                ti.add_if_cond_statement(ts);
            } else if (std::strncmp(iCnd.c_str(), "}elseif(", 8) == 0) {
                std::string condition_str = iCnd.substr(iCnd.find('(') + 1,
                                                        iCnd.find_last_of(')'));
                condition::Condition tc(condition_str, my_service_name);
                ti.set_else_if_condition(tc);
                ti.add_else_if_cond_statement(ts);
            } else if (std::strncmp(iCnd.c_str(), "}else", 5) == 0) {
                ti.add_else_cond_statement(ts);
            }
        } catch (const std::exception& e) {
            throw std::logic_error{"Syntax error in the CLARA routing program."
                                           "Missing parenthesis"};
        }
    } else {
        throw std::logic_error{"Syntax error in the CLARA routing program. "
                                "Malformed conditional statement"};
    }

    return ti;
}

std::string CompositionCompiler::no_blanks(std::string x)
{
    x.erase(std::remove (x.begin(), x.end(), ' '), x.end());
    return x;
}

} // end namespace composition
} // end namespace clara
