//
// Created by Patrick Hennis on 9/25/17.
//

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
