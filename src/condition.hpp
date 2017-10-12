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

#ifndef CLARA_CONDITION_HPP
#define CLARA_CONDITION_HPP

#include <set>
#include <vector>
#include "service_state.hpp"


namespace clara {
namespace condition {

inline std::vector<std::string> tokenize(std::string s, std::string delim)
{
    std:: vector<std::string> tv;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delim)) != std::string::npos) {
        token = s.substr(0, pos);
        tv.push_back(token);
        s.erase(0, pos + delim.length());
    }
    return tv;
}


class Condition {
public:
    Condition();
    Condition(std::string condition_string, std::string service_name);
    std::string get_service_name();
    std::vector<ServiceState::ServiceState> get_and_states();
    std::vector<ServiceState::ServiceState> get_and_not_states();
    std::vector<ServiceState::ServiceState> get_or_states();
    std::vector<ServiceState::ServiceState> get_or_not_states();
    bool is_true(ServiceState::ServiceState owner_ss, ServiceState::ServiceState input_ss);
    std::string to_string();
    bool equals(Condition c);
private:
    std::string service_name_;
    std::vector<ServiceState::ServiceState> and_states;
    std::vector<ServiceState::ServiceState> and_not_states;
    std::vector<ServiceState::ServiceState> or_states;
    std::vector<ServiceState::ServiceState> or_not_states;
    void add_and_state(ServiceState::ServiceState and_state);
    void add_and_not_state(ServiceState::ServiceState and_not_state);
    void add_or_state(ServiceState::ServiceState or_state);
    void add_or_not_state(ServiceState::ServiceState or_state);
    void process(std::string cs);
    void parse_condition(std::string cs, std::string logic_operator);
    bool check_and_condition(std::vector<ServiceState::ServiceState> sc, ServiceState::ServiceState s1, ServiceState::ServiceState s2);
    bool check_or_condition(std::vector<ServiceState::ServiceState> sc, ServiceState::ServiceState s1, ServiceState::ServiceState s2);
};
}
}

#endif //CLARA_CONDITION_HPP
