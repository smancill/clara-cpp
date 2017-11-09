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
#include <algorithm>
#include <regex>


namespace clara {
namespace composition {

    Condition::Condition() {
        this->service_name_ = "default";
    }

    Condition::Condition(const std::string& condition_string,
                         const std::string& service_name) {
        this->service_name_ = service_name;
        process(condition_string);
    }

    std::string Condition::get_service_name() {
        return this->service_name_;
    }

    std::set<ServiceState> Condition::get_and_states() {
        return this->and_states;
    }

    void Condition::add_and_state(const ServiceState& and_state) {
        this->and_states.insert(and_state);
    }

    std::set<ServiceState> Condition::get_and_not_states() {
        return this->and_not_states;
    }

    void Condition::add_and_not_state(const ServiceState& and_not_state) {
        this->and_not_states.insert(and_not_state);
    }

    std::set<ServiceState> Condition::get_or_states() {
        return this->or_states;
    }

    void Condition::add_or_state(const ServiceState& or_state) {
        this->or_states.insert(or_state);
    }

    std::set<ServiceState> Condition::get_or_not_states() {
        return this->or_not_states;
    }

    void Condition::add_or_not_state(const ServiceState& or_state) {
        this->or_not_states.insert(or_state);
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

    void Condition::parse_condition(const std::string& cs,
                                    const std::string& logic_operator) {
        std::vector<std::string> t0;
        std::vector<std::string> t1;

        if (logic_operator == "") {
            if (std::regex_match(cs, composition::CompositionCompiler::get_simp_cond())) {
                if (cs.find("!=") != std::string::npos) {
                    t1 = tokenize(cs, "!=\"");
                    if (t1.size() != 2) {
                        throw std::logic_error{"Condition Exception."};
                    }
                    ServiceState sst(t1[0], t1[1]);
                    add_or_not_state(sst);
                } else if (cs.find("==") != std::string::npos) {
                    t1 = tokenize(cs, "==");
                    if (t1.size() != 2) {
                        throw std::logic_error{"Condition Exception."};
                    }
                    ServiceState sst(t1[0], t1[1]);
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
                    if (std::regex_match(ac, composition::CompositionCompiler::get_simp_cond())) {
                        if (ac.find("!=") != std::string::npos) {
                            t1 = tokenize(t0[0], "!=\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState sst(t1[0], t1[1]);
                            add_and_not_state(sst);
                        } else if (ac.find("==") != std::string::npos) {
                            t1 = tokenize(t0[0], "==\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState sst(t1[0], t1[1]);
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
                    if (std::regex_match(ac, composition::CompositionCompiler::get_simp_cond())) {
                        if (ac.find("!=") != std::string::npos) {
                            t1 = tokenize(t0[0], "!=\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState sst(t1[0], t1[1]);
                            add_or_not_state(sst);
                        } else if (ac.find("==") != std::string::npos) {
                            t1 = tokenize(t0[0], "==\"");
                            if (t1.size() != 2) {
                                throw std::logic_error{"Condition Exception."};
                            }
                            ServiceState sst(t1[0], t1[1]);
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

    bool Condition::check_and_condition(const std::set<ServiceState>& sc,
                                        const ServiceState& s1,
                                        const ServiceState& s2) {

        bool one = false, two = false;
        std::set<ServiceState>::iterator it;

        for(it = sc.begin(); it != sc.end(); ++it)
        {
            auto f = *it;
            if (s1 == f) {
                one = true;
            }
            if (s2 == f) {
                two = true;
            }
        }

        return one && two;
    }

    bool Condition::check_or_condition(const std::set<ServiceState>& sc,
                                       const ServiceState& s1,
                                       const ServiceState& s2) {

        std::set<ServiceState>::iterator it1;
        for (it1 = sc.begin(); it1 != sc.end(); ++it1)
        {
            auto f = *it1;
            if (s1 == f || s2 == f) {
                return true;
            }
        }

        return false;
    }

    bool Condition::is_true(const ServiceState& owner_ss,
                            const ServiceState& input_ss) {
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

    bool Condition::equals(const Condition& c) const {
        if (this->service_name_ == c.service_name_) {
            if (this->or_not_states == c.or_states) {
                if (this->or_not_states == c.or_not_states) {
                    if (this->and_states == c.and_states) {
                        if (this->and_not_states == c.and_not_states) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool Condition::operator<(const Condition& c) const {
        if (this->service_name_ < c.service_name_) {
            if (this->and_states < c.and_states) {
                if (this->and_not_states < c.and_not_states) {
                    if (this->or_states < c.or_states) {
                        if (this->or_not_states < c.or_states) {
                            return true;
                        }
                    }
                }
            }
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
