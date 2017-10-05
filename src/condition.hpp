//
// Created by Patrick Hennis on 9/26/17.
//

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
    std::set<ServiceState::ServiceState> get_and_states();
    std::set<ServiceState::ServiceState> get_and_not_states();
    std::set<ServiceState::ServiceState> get_or_states();
    std::set<ServiceState::ServiceState> get_or_not_states();
    bool is_true(ServiceState::ServiceState owner_ss, ServiceState::ServiceState input_ss);
    std::string to_string();
    bool equals(Condition c);
private:
    std::string service_name_;
    std::set<ServiceState::ServiceState> and_states;
    std::set<ServiceState::ServiceState> and_not_states;
    std::set<ServiceState::ServiceState> or_states;
    std::set<ServiceState::ServiceState> or_not_states;
    void add_and_state(ServiceState::ServiceState and_state);
    void add_and_not_state(ServiceState::ServiceState and_not_state);
    void add_or_state(ServiceState::ServiceState or_state);
    void add_or_not_state(ServiceState::ServiceState or_state);
    void process(std::string cs);
    void parse_condition(std::string cs, std::string logic_operator);
    bool check_and_condition(std::set<ServiceState::ServiceState> sc, ServiceState::ServiceState s1, ServiceState::ServiceState s2);
    bool check_or_condition(std::set<ServiceState::ServiceState> sc, ServiceState::ServiceState s1, ServiceState::ServiceState s2);
};
}
}

#endif //CLARA_CONDITION_HPP
