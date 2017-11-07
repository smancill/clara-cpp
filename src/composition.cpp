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


//    Composition Compiler Classes

// Service State implementation
ServiceState::ServiceState(const std::string& name, const std::string& state) {
    this->state_ = state;
    this->name_ = name;
}

std::string ServiceState::get_name() {
    return name_;
}

std::string ServiceState::get_state() {
    return state_;
}

void ServiceState::set_state(const std::string& state) {
    state_ = state;
}

bool ServiceState::equals(const ServiceState& ss) {
    if (this->name_ == ss.name_ && this->state_ == ss.state_) {
        return true;
    }
    return false;
}

bool ServiceState::operator==(const ServiceState &lhs) {
    return this->get_name() == lhs.name_;
}

int ServiceState::hash_code() {
    int result = std::hash<std::string>{}(name_);
    result = 31 * result + std::hash<std::string>{}(state_);
    return result;
}

bool ServiceState::operator<(const ServiceState& lhs) const {
    return this->name_ < lhs.name_;
}

std::string ServiceState::to_string() {
    return "ServiceState{ name=\'" + name_ + ", state=\'" + state_ + "\'}";
}


//*****************************************
//composition compiler class implementation
//*****************************************

/*
 * IP address regex
 */
        std::string IP = "([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})";

/*
 * String that Starts with a character and can have preceding numbers
 */
        std::string WORD = "([A-Z|a-z]*[0-9]*)";
        std::string PORT = "(.%[0-9]*)*";

/*
 * Service name
 * Format: dpe_name:container_name:engine_name
 */
        std::string SERV_NAME = IP + PORT + "_(java|python|cpp):" + WORD + ":" + WORD;

/*
 * Routing statement Examples:
 *      S1 + S2 + S3;
 *      S1 , S2 + S3;
 *      S1 + S2 , S3;
 *      S1;
 */
        std::string STATEMENT = SERV_NAME+"(,"+SERV_NAME+")*((\\+&?"+SERV_NAME+")*|(\\+"+SERV_NAME+"(,"+SERV_NAME+")*)*)";

// creates regex object out of string
        std::regex Statement_r(STATEMENT);

/*
 * CLARA simple Condition. Example:
 *      Service == "state_name"
 *      Service != "state_name"
 */
        std::string simp_cond_s = SERV_NAME + "(==|!=)" + WORD;
        std::regex SIMP_COND(simp_cond_s);

/*
 * CLARA complex Condition. Example:
 *      Service1 == "state_name1" && Service2 == "state_name2"
 */
        std::string comp_cond_s = simp_cond_s + "((&&|!!)" + simp_cond_s + ")*";
        std::regex COMP_COND(comp_cond_s);


        std::string if_else_if_else = "((if|elseif)\\("+simp_cond_s+"\\)|else)";
        std::regex IEIE_r(if_else_if_else);
/*
 * CLARA conditional statement
 */
        std::string cond_s = "((if|elseif)\\("+simp_cond_s+"\\)\\{\\s*("+STATEMENT+";\\s*)*\\})*(else\\{\\s*("+STATEMENT+";\\s*)*\\})?";
        std::regex COND(cond_s);

        CompositionCompiler::CompositionCompiler(const std::string& service)
                : my_service_name{std::move(service)}
        {
            // nop
        }

        std::regex CompositionCompiler::get_simp_cond() {
            return SIMP_COND;
        }

        void CompositionCompiler::compile(const std::string& iCode)
        {
            // new compile request, clear previous instructions
            reset();

            // remove blanks from string passed in param
            std::string pCode = no_blanks(std::move(iCode));

            /*
             * makes sure that the statement ends with a ';'
             * and then tokenizes the string on ';' chars
             */
            std::vector<std::string> pp = pre_process(pCode);

            int i = -1;
            while (++i < pp.size()) {
                std::string scs1 = pp[i];

                /*
                 * check if conditional statements using
                 * strncmp, checks the specified amount of
                 * chars from the beginning of the given
                 * string, if 0 it is found
                 */
                if (std::strncmp(scs1.c_str(), "if(", 3) == 0
                    || std::strncmp(scs1.c_str(), "}if(", 4) == 0
                    || std::strncmp(scs1.c_str(), "}elseif(", 8) == 0
                    || std::strncmp(scs1.c_str(), "}else", 5) == 0) {

                    Instruction instruction = parse_condition(scs1);

                    // check for nested conditional statements
                    while (++i < pp.size()) {
                        std::string scs2 = pp[i];

                        // using strncmp like before, making sure not found this time
                        if (std::strncmp(scs2.c_str(), "}", 1) != 0
                            && std::strncmp(scs2.c_str(), "if(", 3) != 0
                            && std::strncmp(scs2.c_str(), "}if(", 4) != 0
                            && std::strncmp(scs2.c_str(), "}elseif(", 8) != 0
                            && std::strncmp(scs2.c_str(), "}else", 5) != 0) {

                            /*
                             * check to make sure the instruction name is not
                             * set to null, which is set in default, call parse
                             * conditional statement on the string and instruction
                             */
                            if (instruction.get_service_name() != "null") {
                                parse_conditional_statement(scs2, instruction);
                            }
                        } else {
                            break;
                        }
                    }
                    // add the instruction to the instructions set
                    if (instruction.get_service_name() != "null") {
                        instructions.insert(instruction);
                    }
                    i--;
                } else {
                    // class parse_statement on the statement string
                    parse_statement(scs1);
                }
            }
            if (instructions.empty()) {
                // if nothing has been added to instructions, throw error
                throw std::logic_error{"Composition is irrelevant for a service"};
            }

        }

        void CompositionCompiler::reset()
        {
            CompositionCompiler::instructions.clear();
        }

        std::set<Instruction> CompositionCompiler::get_instructions()
        {
            return instructions;
        }

        std::set<std::string> CompositionCompiler::get_unconditional_links()
        {
            std::set<clara::composition::Instruction>::iterator it;
            std::set<clara::composition::Statement>::iterator inner;

            std::set<std::string> outputs;
            if (!instructions.empty()) {
                for (it = instructions.begin(); it != instructions.end(); ++it) {
                    auto curr = *it;
                    if (!(curr.get_un_cond_statements().empty())) {
                        auto in = curr.get_un_cond_statements();
                        for (inner = in.begin(); inner != in.end(); ++inner) {
                            auto inner_curr = *inner;
                            for (std::string s : inner_curr.get_output_links()) {
                                outputs.insert(s);
                            }
                        }
                    }
                }
            }
            return outputs;
        }

        std::set<std::string> CompositionCompiler::get_links(const ServiceState& owner_ss,
                                                             const ServiceState& input_ss) {
            std::set<std::string> outputs;
            bool in_condition = false;
            bool condition_chosen = false;

            for (Instruction inst : instructions) {
                if (!inst.get_un_cond_statements().empty()) {
                    in_condition = false;
                    for (Statement stmt : inst.get_un_cond_statements()) {
                        for (std::string s : stmt.get_output_links()) {
                            outputs.insert(s);
                        }
                    }
                    continue;
                }

                if (!inst.get_if_condition().get_service_name().empty()) {
                    in_condition = true;
                    condition_chosen = false;
                    if (inst.get_if_condition().is_true(owner_ss, input_ss)) {
                        condition_chosen = true;
                        for (Statement st : inst.get_if_cond_statements()) {
                            for (std::string s : st.get_output_links()) {
                                outputs.insert(s);
                            }
                        }
                    }
                    continue;
                }

                if (in_condition && !condition_chosen) {
                    if (!inst.get_else_if_condition().get_service_name().empty()) {
                        if (inst.get_else_if_condition().is_true(owner_ss, input_ss)) {
                            condition_chosen = true;
                            for (Statement stmt : inst.get_else_if_cond_statements()) {
                                for (std::string s : stmt.get_output_links()) {
                                    outputs.insert(s);
                                }
                            }
                        }
                        continue;
                    }

                    if (!inst.get_else_cond_statements().empty()) {
                        condition_chosen = true;
                        for (Statement stmt : inst.get_else_cond_statements()) {
                            for (std::string s : stmt.get_output_links()) {
                                outputs.insert(s);
                            }
                        }
                    }
                }
            }
            return outputs;
        }

        std::vector<std::string> CompositionCompiler::pre_process(const std::string& pCode)
        {
            if (pCode.find(';') == std::string::npos && pCode.back() != ';') {
                throw std::logic_error{"Syntax error in the CLARA routing program. "
                                               "Missing end of statement operator = \";\""};
            }

            std::vector<std::string> r;
            std::vector<std::string> st = tokenize(pCode, ";");

            for (std::string text : st) {
                if (text != "") {
                    r.push_back(text);
                }
            }

            return r;

        }

        bool CompositionCompiler::parse_statement(std::string iStmt)
        {
            bool b = false;
            Instruction ti(iStmt);
            iStmt = remove_first(iStmt, '}');

            try {
                if (std::regex_match(iStmt, Statement_r)) {
                    if (iStmt.find(my_service_name) == std::string::npos) {
                        return false;
                    }

                    Statement ts(iStmt, my_service_name);
                    ti.add_un_cond_statement(ts);
                    instructions.insert(ti);
                    b = true;
                }
                else if (std::regex_match(iStmt, SIMP_COND)) {
                    throw std::logic_error{"Syntax error: just a simple condition"};
                }
                else {
                    std::cout << "DDD ----- > statement = " + iStmt << std::endl;
                    throw std::logic_error{"Syntax error in the CLARA routing program. "
                                                   "Malformed routing statement"};
                }
            } catch (const std::logic_error& e) {
                std::cout << e.what() << std::endl;
            }
            return b;
        }

        bool CompositionCompiler::parse_conditional_statement(const std::string& iStmt,
                                                              Instruction& ti)
        {
            if (std::regex_match(iStmt, Statement_r)) {
                if (iStmt.find(my_service_name) == std::string::npos) {
                    return false;
                }

                Statement ts(iStmt, my_service_name);

                // make a default instruction for comparison
                Instruction di("default_instruction");
                if (ti.get_if_condition().equals(di.get_if_condition())) {
                    ti.add_if_cond_statement(ts);
                }
                else if (ti.get_else_if_condition().equals(di.get_else_if_condition())) {
                    ti.add_else_if_cond_statement(ts);
                }
                else {
                    ti.add_else_cond_statement(ts);
                }
                return true;
            }
            else {
                std::cout << "DDD ----- > statement = " + iStmt << std::endl;
                throw std::logic_error{"Syntax error in the CLARA routing program."
                                               "Malformed routing statement"};
            }
        }

        Instruction CompositionCompiler::parse_condition(const std::string& iCnd)
        {
            Instruction ti(my_service_name);

            std::string tmp = iCnd.substr(0, iCnd.find(")") + 1);

            if (std::regex_match(tmp, IEIE_r)) {
                try {
                    std::string statement_str = iCnd.substr(iCnd.find('{')+1);
                    statement_str = statement_str.substr(0, statement_str.size()-1);

                    if (statement_str.find(my_service_name) == std::string::npos) {
                        return Instruction("null");
                    }

                    Statement ts(statement_str, my_service_name);

                    if (std::strncmp(iCnd.c_str(), "}if(", 4) == 0 ||
                        strncmp(iCnd.c_str(), "if(", 3) == 0) {
                        std::string condition_str =
                                iCnd.substr(iCnd.find('(') + 1);
                        condition_str = condition_str.substr(0, condition_str.find(')'));
                        Condition tc(condition_str, my_service_name);
                        ti.set_if_condition(tc);
                        ti.add_if_cond_statement(ts);
                    }
                    else if (std::strncmp(iCnd.c_str(), "}elseif(", 8) == 0) {
                        std::string condition_str = iCnd.substr(iCnd.find('(') + 1,
                                                                iCnd.find_last_of(')'));
                        Condition tc(condition_str, my_service_name);
                        ti.set_else_if_condition(tc);
                        ti.add_else_if_cond_statement(ts);
                    }
                    else if (std::strncmp(iCnd.c_str(), "}else", 5) == 0) {
                        ti.add_else_cond_statement(ts);
                    }
                } catch (const std::exception& e) {
                    throw std::logic_error{"Syntax error in the CLARA routing program."
                                                   "Missing parenthesis"};
                }
            }
            else {
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

        std::string CompositionCompiler::test_regex(std::string s) {
            if (std::regex_match(s, Statement_r)) {
                return "STATEMENT";
            }
            if (std::regex_match(s, SIMP_COND)) {
                return "SIMP_COND";
            }
            if (std::regex_match(s, COMP_COND)) {
                return "COMP_COND";
            }
            if (std::regex_match(s, COND)) {
                return "COND";
            }
            else {
                return "did not confine to a regex statement";
            }
        }


} // end namespace composition
} // end namespace clara
