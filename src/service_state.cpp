//
// Created by Patrick Hennis on 9/27/17.
//

#include "service_state.hpp"

namespace clara {
namespace ServiceState {

ServiceState::ServiceState(std::string name, std::string state) {
    this->state_ = state;
    this->name_ = name;
}

std::string ServiceState::get_name() {
    return name_;
}

std::string ServiceState::get_state() {
    return state_;
}

void ServiceState::set_state(std::string state) {
    state_ = state;
}

bool ServiceState::equals() {
    
}

int ServiceState::hash_code() {

}

std::string ServiceState::to_string() {
    return "ServiceState{ name=\'" + name_ + ", state=\'" + state_ + "\'}";
}



}
}