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

#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace clara {
namespace composition {

//    Simple Compiler

SimpleCompiler::SimpleCompiler(std::string service_name)
  : service_name_{std::move(service_name)}
{
    // nop
}

void SimpleCompiler::compile(const std::string& composition)
{
    prev_.clear();
    next_.clear();

    auto sub_composition = &prev_;
    bool service_found = false;

    std::stringstream ss{composition};
    std::string service{};
    while (std::getline(ss, service, '+')) {
        if (service.back() == ';') {
            service.pop_back();
        }
        if (service == service_name_) {
            sub_composition = &next_;
            service_found = true;
            continue;
        }
        sub_composition->push_back(service);
    }
    if (!service_found) {
        throw std::logic_error{service_name_ + " not in: " + composition};
    }
}

std::set<std::string> SimpleCompiler::outputs()
{
    std::set<std::string> out;
    if (!next_.empty()) {
        out.insert(next_.front());
    }
    return out;
}


//    Composition Compiler

CompositionCompiler::CompositionCompiler(std::string service)
    : my_service_name{std::move(service)}
{
    // nop
}

void CompositionCompiler::compile(std::string iCode)
{
    reset();

    std::string pCode = no_blanks(iCode);

    std::set<std::string> pp = pre_process(pCode);
}

void CompositionCompiler::reset()
{
    CompositionCompiler::intructions.clear();
}

std::set<Instruction> CompositionCompiler::get_instructions()
{
    return intructions;
}

std::set<std::string> CompositionCompiler::get_unconditional_links()
{

}

//std::set<std::string> CompositionCompiler::get_links(Service owner_ss, Service input_ss)
//{
//
//}

std::set<std::string> CompositionCompiler::pre_process(std::string pCode)
{
    if (pCode.find(';') == std::string::npos && pCode.back() != ';') {
        throw std::logic_error{"Syntax error in the CLARA routing program. "
                    "Missing end of statement operator = \";\""};
    }

    std::set<std::string> r;
    std::vector<std::string> st = tokenize(pCode, ";");

    for (std::string text : st) {
        if (text != "" && text != "}") {
            r.insert(text);
        }
    }

    return r;

}

bool CompositionCompiler::parse_statement(std::string iStmt)
{

}

bool CompositionCompiler::parse_conditional_statement(std::string iStmt, Instruction ti)
{

}

Instruction CompositionCompiler::parse_condition(std::string iCnd)
{

}

std::string CompositionCompiler::no_blanks(std::string x)
{
    x.erase(std::remove (x.begin(), x.end(), ' '), x.end());
    return x;
}

} // end namespace composition
} // end namespace clara
