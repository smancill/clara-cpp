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

#ifndef CLARA_INSTRUCTION_HPP
#define CLARA_INSTRUCTION_HPP

#include "condition.hpp"
#include "statement.hpp"
#include <set>

namespace clara {
namespace instruction {

class Instruction {
public:
    Instruction(std::string service_name);
    std::string get_service_name();

    condition::Condition get_if_condition();
    void set_if_condition(condition::Condition if_condition);
    std::vector<statement::Statement> get_if_cond_statements();
    void set_if_cond_statements(std::vector<statement::Statement> if_cond_statements);
    void add_if_cond_statement(statement::Statement if_cond_statement);

    condition::Condition get_else_if_condition();
    void set_else_if_condition(condition::Condition else_if_condition);
    std::vector<statement::Statement> get_else_if_cond_statements();
    void set_else_if_cond_statements(std::vector<statement::Statement> else_if_cond_statements);
    void add_else_if_cond_statement(statement::Statement else_if_cond_statement);

    std::vector<statement::Statement> get_else_cond_statements();
    void set_else_cond_statements(std::vector<statement::Statement> else_cond_statements);
    void add_else_cond_statement(statement::Statement else_cond_statement);

    std::vector<statement::Statement> get_un_cond_statements();
    void set_un_cond_statements(std::vector<statement::Statement> un_cond_statements);
    void add_un_cond_statement(statement::Statement un_cond_statement);

    std::string to_string();
    bool equals(Instruction i);
    int hash_code();
    bool operator<(const Instruction& lhs);

private:
    condition::Condition if_condition;
    std::vector<statement::Statement> if_cond_statements;
    condition::Condition else_if_condition;
    std::vector<statement::Statement> else_if_cond_statements;
    std::vector<statement::Statement> else_cond_statements;
    std::vector<statement::Statement> un_cond_statements;
    std::string service_name_;
};

}
}


#endif //CLARA_INSTRUCTION_HPP
