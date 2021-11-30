/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_COMPOSITION_COMPILER_HPP
#define CLARA_COMPOSITION_COMPILER_HPP

#include <list>
#include <set>
#include <string>

namespace clara::composition {

class SimpleCompiler
{
public:
    SimpleCompiler(std::string service_name);

    void compile(const std::string& composition);

    auto outputs() -> std::set<std::string>;

private:
    std::string service_name_;
    std::list<std::string> prev_;
    std::list<std::string> next_;
};

} // end namespace clara::composition

#endif // end of include guard: CLARA_COMPOSITION_COMPILER_HPP
