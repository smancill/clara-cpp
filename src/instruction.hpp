//
// Created by Patrick Hennis on 9/25/17.
//

#ifndef CLARA_INSTRUCTION_HPP
#define CLARA_INSTRUCTION_HPP

#include "condition.hpp"
#include "statement.hpp"

namespace clara {
namespace instruction {

class Instruction {
public:
    Instruction(std::string service_name);
    std::string get_service_name();

    Condition get_if_condition();
    void set_if_condition(Condition if_condition);
    std::set<Statement> get_if_cond_statements();
    void set_if_cond_statements(std::set<Statement> if_cond_statements);
    void add_if_cond_statement(Statement if_cond_statement);

    Condition get_else_if_condition();
    void set_else_if_condition(Condition else_if_condition);
    std::set<Statement> get_else_if_cond_statements();
    void set_else_if_cond_statements(std::set<Statement> else_if_cond_statements);
    void add_else_if_cond_statement(Statement else_if_cond_statement);

    std::set<Statement> get_else_cond_statements();
    void set_else_cond_statements(std::set<Statement> else_cond_statements);
    void add_else_cond_statement(Statement else_cond_statement);

    std::set<Statement> get_un_cond_statements();
    void set_un_cond_statements(Set<Statement> un_cond_statements);
    void add_un_cond_statement(Statement un_cond_statement);

    std::string to_string();

private:
    Condition if_condition;
    std::set<Statement> if_cond_statements;

    Condition else_if_condition;
    std::set<Statement> else_if_cond_statements;

    std::set<Statement> else_cond_statements;

    std::set<Statement> un_cond_statements;

    std::string service;
};

}
}


#endif //CLARA_INSTRUCTION_HPP
