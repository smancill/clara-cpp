#include <clara/msg/utils.hpp>

#include <iostream>

namespace util = clara::msg::util;


int main(int argc, char** argv)
{
    std::cout << "-- All local addresses" << std::endl;
    for (const auto& a : util::get_localhost_addrs()) {
        std::cout << a << std::endl;
    }
    std::cout << std::endl;

    std::cout << "-- Resolve \"localhost\"" << std::endl;
    std::cout << util::localhost() << std::endl;
    std::cout << util::to_host_addr("localhost") << std::endl;

    if (argc == 2) {
        std::cout << std::endl;
        std::cout << "-- Resolve \"" << argv[1] << "\"" << std::endl;
        std::cout << util::to_host_addr(argv[1]) << std::endl;
    }

    return 0;
}
