//
// Created by Patrick Hennis on 9/25/17.
//

#ifndef CLARA_COMPOSITION_PARSER_HPP
#define CLARA_COMPOSITION_PARSER_HPP

#include <string>

namespace clara {
namespace parser {

class CompositionParser{
public:
    CompositionParser();
    std::string remove_first(std::string s);
    std::string remove_first(std::string input, std::string first_char);
    std::string remove_last(std::string s);
    std::string get_first_service(std::string composition);
    std::string get_j_set_element_at(std::vector set, int index);
};

}
}

#endif //CLARA_COMPOSITION_PARSER_HPP
