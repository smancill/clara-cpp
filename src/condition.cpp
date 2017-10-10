//
// Created by Patrick Hennis on 9/26/17.
//

#include "condition.hpp"
#include "composition.hpp"
#include <algorithm>
#include <regex>


namespace clara {
namespace condition {

    Condition::Condition() {
        this->service_name_ = "default";
    }

    Condition::Condition(std::string condition_string, std::string service_name) {
        this->service_name_ = service_name;
        process(condition_string);
    }

    std::string Condition::get_service_name() {
        return this->service_name_;
    }

    std::vector<ServiceState::ServiceState> Condition::get_and_states() {
        return this->and_states;
    }

    void Condition::add_and_state(ServiceState::ServiceState and_state) {
        this->and_states.push_back(and_state);
    }

    std::vector<ServiceState::ServiceState> Condition::get_and_not_states() {
        return this->and_not_states;
    }

    void Condition::add_and_not_state(ServiceState::ServiceState and_not_state) {
        this->and_not_states.push_back(and_not_state);
    }

    std::vector<ServiceState::ServiceState> Condition::get_or_states() {
        return this->or_states;
    }

    void Condition::add_or_state(ServiceState::ServiceState or_state) {
        this->or_states.push_back(or_state);
    }

    std::vector<ServiceState::ServiceState> Condition::get_or_not_states() {
        return this->or_not_states;
    }

    void Condition::add_or_not_state(ServiceState::ServiceState or_state) {
        this->or_not_states.push_back(or_state);
    }

    void Condition::process(std::string cs) {
        if (cs.find("(") != std::string::npos) {
            std::replace( cs.begin(), cs.end(), '(', ' ');
        }
        if (cs.find(")") != std::string::npos) {
            std::replace(cs.begin(), cs.end(), ')', ' ');
        }

        if (cs.find("&&") != std::string::npos)  {
            parse_condition(cs, "&&");
        } else if (cs.find("!!") != std::string::npos) {
            parse_condition(cs, "!!");
        } else {
            parse_condition(cs, "");
        }
    }

    // todo : make sure std::vector "at()" works as intended and exceptions are fine
    void Condition::parse_condition(std::string cs, std::string logic_operator) {
        std::vector<std::string> t0;
        std::vector<std::string> t1;
        std::smatch matcher;

        if (logic_operator == "") {
            if (std::regex_search(cs, matcher, composition::CompositionCompiler::get_simp_cond())) {
                if (cs.find("!=") != std::string::npos) {
                    t1 = tokenize(cs, "!=\"");
                    if (t1.size() != 2) {
                        throw std::logic_error{"Condition Exception."};
                    }
                    ServiceState::ServiceState sst(t1.at(0), t1.at(1));
                    add_or_not_state(sst);
                } else if (cs.find("==") != std::string::npos) {
                    t1 = tokenize(cs, "==\"");
                    if (t1.size() != 2) {
                        throw std::logic_error{"Condition Exception."};
                    }
                    ServiceState::ServiceState sst(t1.at(0), t1.at(1));
                    add_or_state(sst);
                } else {
                    throw std::logic_error{"Condition Exception"};
                }
            } else {
                throw std::logic_error{"Condition Exception"};
            }
        } else {

            if (cs.find("&&") != std::string::npos && cs.find("!!") == std::string::npos) {
                t0 = tokenize(cs, logic_operator);
                for (std::string ac : t0) {
                    if (std::regex_search(ac, matcher, composition::CompositionCompiler::get_simp_cond())) {
                        if (ac.find("!=") != std::string::npos) {
                            t1 = tokenize(t0.at(0), "!=\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState::ServiceState sst(t1.at(0), t1.at(1));
                            add_and_not_state(sst);
                        } else if (ac.find("==") != std::string::npos) {
                            t1 = tokenize(t0.at(0), "==\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState::ServiceState sst(t1.at(0), t1.at(1));
                            add_and_state(sst);
                        } else {
                            throw std::logic_error{"Condition Exception"};
                        }
                    } else {
                        throw std::logic_error{"Condition Exception"};
                    }
                }
            } else if (cs.find("!!") != std::string::npos && cs.find("&&") == std::string::npos) {
                t0 = tokenize(cs, logic_operator);
                for (std::string ac : t0) {
                    if (std::regex_search(ac, matcher, composition::CompositionCompiler::get_simp_cond())) {
                        if (ac.find("!=") != std::string::npos) {
                            t1 = tokenize(t0.at(0), "!=\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState::ServiceState sst(t1.at(0), t1.at(1));
                            add_or_not_state(sst);
                        } else if (ac.find("==") != std::string::npos) {
                            t1 = tokenize(t0.at(0), "==\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState::ServiceState sst(t1.at(0), t1.at(1));
                            add_or_state(sst);
                        } else {
                            throw std::logic_error{"Condition Exception"};
                        }
                    } else {
                        throw std::logic_error{"Condition Exception"};
                    }
                }
            } else {
                throw std::logic_error{"Condition Exception"};
            }
        }
    }

    bool Condition::check_and_condition(std::vector<ServiceState::ServiceState> sc, ServiceState::ServiceState s1,
                                        ServiceState::ServiceState s2) {
        if (std::find(sc.begin(), sc.end(), s1) != sc.end()) {
            if (std::find(sc.begin(), sc.end(), s2) != sc.end()) {
                return true;
            }
        }
        return false;
    }

    bool Condition::check_or_condition(std::vector<ServiceState::ServiceState> sc, ServiceState::ServiceState s1,
                                       ServiceState::ServiceState s2) {
        if (std::find(sc.begin(), sc.end(), s1) != sc.end()) {
            return true;
        }
        if (std::find(sc.begin(), sc.end(), s2) != sc.end()) {
            return true;
        }
        return false;
    }

    bool Condition::is_true(ServiceState::ServiceState owner_ss, ServiceState::ServiceState input_ss) {
        bool check_and = get_and_states().empty() ||
                check_and_condition(get_and_states(), owner_ss, input_ss);
        bool check_and_not = get_and_not_states().empty() ||
                 check_and_condition(get_and_not_states(), owner_ss, input_ss);
        bool check_or = get_or_states().empty() ||
                         check_or_condition(get_or_states(), owner_ss, input_ss);
        bool check_or_not = get_or_not_states().empty() ||
                             check_or_condition(get_or_not_states(), owner_ss, input_ss);

        return check_and && check_and_not && check_or && check_or_not;
    }

    bool Condition::equals(Condition c) {
        if (this->service_name_ == c.get_service_name()) {
//            if (this->or_not_states == c.get_or_states()) {
//                if (this->or_not_states == c.get_or_not_states()) {
//                    if (this->and_states == c.get_and_states()) {
//                        if (this->and_not_states == c.get_and_not_states()) {
                            return true;
//                        }
//                    }
//                }
//            }
        }
        return false;
    }

    std::string Condition::to_string() {
        return "Condition{"
               "serviceName='" + service_name_ + '\''
//               + ", andStates=" + and_states
//               + ", andNotStates=" + and_not_states
//               + ", orStates=" + or_states
//               + ", orNotStates=" + or_not_states
               + '}';
    }

}
}
