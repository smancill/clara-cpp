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

namespace clara {
namespace composition {

inline std::vector<std::string> tokenize(std::string s, std::string delim)
{
    std:: vector<std::string> tv;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delim)) != std::string::npos) {
        if (std::strncmp(s.c_str(), "if(", 3) == 0
            || std::strncmp(s.c_str(), "}if(", 4) == 0
            || std::strncmp(s.c_str(), "elseif(", 7) == 0
            || std::strncmp(s.c_str(), "else", 4) == 0) {
            pos = s.find("}");
            token = s.substr(0, pos+1);
            tv.push_back(token);
            s.erase(0, pos + 1);
        } else {
            token = s.substr(0, pos);
            tv.push_back(token);
            s.erase(0, pos + delim.length());
        }
    }
    tv.push_back(s);
    return tv;
}

inline std::string remove_first(std::string str, char d) {
    auto pos = str.find(d);
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
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

// classes for Composition Compiler

class ServiceState {
public:
    ServiceState(const std::string& name, const std::string& state);
    std::string get_name();
    std::string get_state();
    void set_state(const std::string& state);
    bool equals(const ServiceState& ss);
    int hash_code();
    std::string to_string();
    bool operator==(const ServiceState& lhs) const;
    bool operator<(const ServiceState& lhs) const;
private:
    std::string name_;
    std::string state_;
};

class Condition {
public:
    Condition();
    Condition(const std::string& condition_string, const std::string& service_name);
    std::string get_service_name();
    std::set<ServiceState> get_and_states();
    std::set<ServiceState> get_and_not_states();
    std::set<ServiceState> get_or_states();
    std::set<ServiceState> get_or_not_states();
    bool is_true(const ServiceState& owner_ss, const ServiceState& input_ss);
    std::string to_string();
    bool equals(const Condition& c) const;
    bool operator<(const Condition& c) const;
private:
    std::string service_name_;
    std::set<ServiceState> and_states;
    std::set<ServiceState> and_not_states;
    std::set<ServiceState> or_states;
    std::set<ServiceState> or_not_states;
    void add_and_state(const ServiceState& and_state);
    void add_and_not_state(const ServiceState& and_not_state);
    void add_or_state(const ServiceState& or_state);
    void add_or_not_state(const ServiceState& or_state);
    void process(std::string cs);
    void parse_condition(const std::string& cs, const std::string& logic_operator);
    bool check_and_condition(const std::set<ServiceState>& sc, const ServiceState& s1, const ServiceState& s2);
    bool check_or_condition(const std::set<ServiceState>& sc, const ServiceState& s1, const ServiceState& s2);
};

class Statement {
public:
    Statement(const std::string& statement_string, const std::string& service_name);
    std::string get_service_name();
    std::string get_statement_string();
    std::set<std::string> get_input_links();
    std::set<std::string> get_output_links();
    std::set<std::string> get_log_and_inputs();
    std::string to_string();
    int hash_code();
    bool operator<(const Statement& lhs) const;
    bool operator==(const Statement& lhs) const;

private:
    std::string service_name_;
    std::string statement_string_;
    std::set<std::string> log_and_inputs;
    std::set<std::string> input_links;
    std::set<std::string> output_links;

    void process(const std::string& statement);
    void parse_linked(const std::string& service_name, const std::string& statement);
    bool is_log_and(const std::string& service_name, const std::string& composition);
};

class Instruction {
public:
    Instruction(const std::string& service_name);
    std::string get_service_name();

    Condition get_if_condition();
    void set_if_condition(const Condition& if_condition);
    std::set<Statement> get_if_cond_statements();
    void set_if_cond_statements(const std::set<Statement>& if_cond_statements);
    void add_if_cond_statement(const Statement& if_cond_statement);

    Condition get_else_if_condition();
    void set_else_if_condition(const Condition& else_if_condition);
    std::set<Statement> get_else_if_cond_statements();
    void set_else_if_cond_statements(const std::set<Statement>& else_if_cond_statements);
    void add_else_if_cond_statement(const Statement& else_if_cond_statement);

    std::set<Statement> get_else_cond_statements();
    void set_else_cond_statements(const std::set<Statement>& else_cond_statements);
    void add_else_cond_statement(const Statement& else_cond_statement);

    std::set<Statement> get_un_cond_statements();
    void set_un_cond_statements(const std::set<Statement>& un_cond_statements);
    void add_un_cond_statement(const Statement& un_cond_statement);

    std::string to_string();
    int hash_code();
    bool operator<(const Instruction& lhs) const;
    bool operator==(const Instruction& lhs) const;

private:
    Condition if_condition;
    std::set<Statement> if_cond_statements;
    Condition else_if_condition;
    std::set<Statement> else_if_cond_statements;
    std::set<Statement> else_cond_statements;
    std::set<Statement> un_cond_statements;
    std::string service_name_;
};

class CompositionCompiler
{
public:
    CompositionCompiler(const std::string& service);
    void compile(const std::string& iCode);
    void reset();
    std::set<Instruction> get_instructions();
    std::set<std::string> get_unconditional_links();
    std::set<std::string> get_links(const ServiceState& owner_ss, const ServiceState& input_ss);
    std::set<std::string> get_output_links();
    std::set<std::string> get_input_links();
    static std::regex get_simp_cond();

private:
    std::string STR, STR2, Sn, RStmt, sCond, cCond, Cond;
    std::set<Instruction> instructions;
    std::string my_service_name;
    std::vector<std::string> pre_process(const std::string& pCode);
    bool parse_statement(std::string iStmt);
    bool parse_conditional_statement(const std::string& iStmt, Instruction& ti);
    Instruction parse_condition(const std::string& iCnd);
    std::string no_blanks(std::string x);
};

} // end namespace composition
} // end namespace clara

#endif // end of include guard: CLARA_COMPOSITION_COMPILER_HPP
