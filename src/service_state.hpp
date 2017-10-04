//
// Created by Patrick Hennis on 9/27/17.
//

#ifndef CLARA_SERVICE_STATE_HPP
#define CLARA_SERVICE_STATE_HPP

#include <string>

namespace clara {
namespace ServiceState {


class ServiceState {
public:
    ServiceState(std::string name, std::string state);
    std::string get_name();
    std::string get_state();
    void set_state(std::string state);
    bool equals();
    int hash_code();
    std::string to_string();
private:
    std::string name_;
    std::string state_;
};

}
}


#endif //CLARA_SERVICE_STATE_HPP
