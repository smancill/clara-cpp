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
#ifndef CLARA_STATEMENT_HPP
#define CLARA_STATEMENT_HPP

#include "engine_data_helper.hpp"
#include <set>
#include <map>

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

inline std::string remove_first(std::string str, char d) {
    auto pos = str.find(d);
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

namespace clara {
namespace statement {

class Statement {
public:
    Statement(std::string statement_string, std::string service_name);
    std::string get_service_name();
    std::string get_statement_string();
    std::set<std::string> get_input_links();
    std::set<std::string> get_output_links();
    std::map<std::string, EngineData>get_log_and_inputs();
    std::string to_string();
    bool equals(Statement s);
    int hash_code();

private:
    std::string service_name_;
    std::string statement_string_;
    std::map<std::string, clara::EngineData> log_and_inputs;
    std::set<std::string> input_links;
    std::set<std::string> output_links;

    void process(std::string statement);
    void parse_linked(std::string service_name, std::string statement);
    bool is_log_and(std::string service_name, std::string composition);


};

}
}


#endif //CLARA_STATEMENT_HPP
