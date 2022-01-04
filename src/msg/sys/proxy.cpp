/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/proxy.hpp>

#include <clara/msg/utils.hpp>

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>

#include <cxxopts.hpp>

#include <unistd.h>

namespace clara::msg::util {

auto get_current_time() -> std::string;

}


static volatile sig_atomic_t signal_value = 0;


static void signal_handler(int signal)
{
    signal_value = signal;
}


static void wait_signals()
{
    struct sigaction action;

    action.sa_handler = signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);

    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);

    pause();

    if (signal_value == SIGINT) {
        std::cout << std::endl;
    }
    std::cout << "Exiting..." << std::endl;
}


using namespace clara::msg;


auto get_address(const cxxopts::ParseResult& result) -> ProxyAddress
{
    auto host = util::localhost();
    auto port = ProxyAddress::default_port;

    if (result.count("host") > 0) {
        host = result["host"].as<std::string>();
        host = util::to_host_addr(host);
    }
    if (result.count("port") > 0) {
        port = result["port"].as<int>();
    }

    return {host, port};
}


auto options_parser() -> cxxopts::Options
{
    using namespace cxxopts;

    auto options = Options{"c_proxy", "Clara C++ pub-sub proxy\n"};
    options.add_options()
        ("host", "use the given host address", value<std::string>())
        ("port", "use the given port", value<int>())
        ("h,help", "Print usage");

    return options;
}


int main(int argc, char** argv)
{
    try {
        auto options = options_parser();
        auto result = options.parse(argc, argv);
        if (result["help"].count() > 0) {
            std::cout << options.help() << std::endl;
            return EXIT_SUCCESS;
        }

        auto addr = get_address(result);
        auto proxy = sys::Proxy{addr};
        proxy.start();

        printf("[%s] Clara proxy INFO: running on host = %s  port = %d\n",
               util::get_current_time().c_str(), addr.host().c_str(), addr.pub_port());

        wait_signals();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
