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

bool ServiceState::equals(ServiceState ss) {
    if (this->name_ == ss.name_ && this->state_ == ss.state_) {
        return true;
    }
    return false;
}

bool ServiceState::operator==(const ServiceState lhs) {
    return this->get_name() == lhs.name_;
}

int ServiceState::hash_code() {
    int result = std::hash<std::string>{}(name_);
    result = 31 * result + std::hash<std::string>{}(state_);
    return result;
}

std::string ServiceState::to_string() {
    return "ServiceState{ name=\'" + name_ + ", state=\'" + state_ + "\'}";
}

}
}
