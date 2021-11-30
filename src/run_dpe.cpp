/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dpe.hpp"
#include "dpe_options.hpp"

#include <clara/msg/context.hpp>

#include <csignal>
#include <iostream>
#include <string>

#include <unistd.h>


volatile sig_atomic_t quit;

void signal_handler(int sig)
{
    std::cout << std::endl;
    signal(sig, signal_handler);
    quit = 1;
}


int main(int argc, char** argv)
{
    auto options = clara::DpeOptionsParser{};
    if (!options.parse(argc, argv)) {
        return EXIT_FAILURE;
    }
    if (options.has_help()) {
        options.print_help();
        return EXIT_SUCCESS;
    }

    auto ctx = clara::msg::Context::instance();
    ctx->set_io_threads(options.io_threads());
    ctx->set_max_sockets(options.max_sockets());

    clara::Dpe dpe{false,
                   options.local_address(),
                   options.frontend_address(),
                   options.config()};
    dpe.start();

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while (quit == 0) {
        sleep(1);
    }

    return (EXIT_SUCCESS);
};
