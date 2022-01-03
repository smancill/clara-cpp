/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/actor.hpp>
#include <clara/msg/utils.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace cm = clara::msg;


int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cerr << "usage: remote_thr <bind-to> <message-size> <message-count>" << std::endl;
        return EXIT_FAILURE;
    }

    const auto bind_to = cm::util::to_host_addr(argv[1]);
    const auto message_size = std::stoi(argv[2]);
    const auto message_count = std::stol(argv[3]);

    try {
        auto publisher = cm::Actor("thr_publisher");
        auto connection = publisher.connect(cm::ProxyAddress{bind_to});

        auto topic = cm::Topic::raw("thr_topic");
        auto data = std::vector<std::uint8_t>(message_size);
        auto msg = cm::Message{topic, "data/binary", data};

        std::cout << "Publishing messages..." << std::endl;
        for (int i = 0; i < message_count; ++i) {
            publisher.publish(connection, msg);
        }
        std::cout << "Done!" << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
