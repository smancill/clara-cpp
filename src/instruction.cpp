//
// Created by Patrick Hennis on 9/25/17.
//

#include "instruction.hpp"

namespace clara {
    namespace instruction {

        Instruction::Instruction(std::string service_name) {
            this->service_name_ = service_name;
        }

        std::string Instruction::get_service_name() {
            return service_name_;
        }

        condition::Condition Instruction::get_if_condition() {
            return if_condition;
        }

        void Instruction::set_if_condition(condition::Condition if_condition) {
            this->if_condition = if_condition;
        }

        void Instruction::set_if_cond_statements(std::vector<statement::Statement> if_cond_statements) {
            this->if_cond_statements = if_cond_statements;
        }

        std::vector<statement::Statement> Instruction::get_if_cond_statements() {
            return if_cond_statements;
        }

        void Instruction::add_if_cond_statement(statement::Statement if_cond_statement) {
            this->if_cond_statements.push_back(if_cond_statement);
        }

        condition::Condition Instruction::get_else_if_condition() {
            return else_if_condition;
        }

        void Instruction::set_else_if_condition(condition::Condition else_if_condition) {
            this->else_if_condition = else_if_condition;
        }

        std::vector<statement::Statement> Instruction::get_else_if_cond_statements() {
            return else_if_cond_statements;
        }

        void Instruction::set_else_if_cond_statements(std::vector<statement::Statement> else_if_cond_statements) {
            this->else_if_cond_statements = else_if_cond_statements;
        }

        void Instruction::add_else_if_cond_statement(statement::Statement else_if_cond_statement) {
            this->else_if_cond_statements.push_back(else_if_cond_statement);
        }

        std::vector<statement::Statement> Instruction::get_else_cond_statements() {
            return else_if_cond_statements;
        }

        void Instruction::set_else_cond_statements(std::vector<statement::Statement> else_cond_statements) {
            this->else_cond_statements = else_cond_statements;
        }

        void Instruction::add_else_cond_statement(statement::Statement else_cond_statement) {
            this->else_cond_statements.push_back(else_cond_statement);
        }

        std::vector<statement::Statement> Instruction::get_un_cond_statements() {
            return un_cond_statements;
        }

        void Instruction::set_un_cond_statements(std::vector<statement::Statement> un_cond_statements) {
            this->un_cond_statements = un_cond_statements;
        }

        void Instruction::add_un_cond_statement(statement::Statement un_cond_statement) {
            this->un_cond_statements.push_back(un_cond_statement);
        }

        std::string Instruction::to_string() {
            return "Instruction{"
//                   "ifCondition=" + if_condition
//                   + ", ifCondStatements=" + if_cond_statements
//                   + ", elseifCondition=" + else_if_condition
//                   + ", elseifCondStatements=" + else_if_cond_statements
//                   + ", elseCondStatements=" + else_cond_statements
//                   + ", unCondStatements=" + un_cond_statements
                   ", serviceName='" + service_name_ + '\''
                   + '}';
        }

        bool Instruction::equals(Instruction i) {
            if (this->service_name_ != i.service_name_) {
                return false;
            }

//            if (!(this->if_condition.equals(i.if_condition))) {
//                return false;
//            }

//            if (this->if_cond_statements != i.if_cond_statements) {
//                return false;
//            }

//            if (!(this->else_if_condition.equals(i.else_if_condition))) {
//                return false;
//            }

//            if (this->else_if_cond_statements != i.else_if_cond_statements) {
//                return false;
//            }
//
//            if (this->else_cond_statements != i.else_cond_statements) {
//                return false;
//            }
//
//            if (this->un_cond_statements != i.un_cond_statements) {
//                return false;
//            }

            return true;
        }

        int Instruction::hash_code() {
            return 0;
        }

    }
}
