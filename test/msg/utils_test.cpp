/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/utils.hpp>

#include "helper/utils.hpp"

#include <gmock/gmock.h>

#include <thread>
#include <utility>

using namespace clara::msg;
using namespace testing;


using AddrParam = std::pair<std::string, bool>;

PARAMETERIZED_SUITE(IpAddrSuite, AddrParam)
{
    return {
        // valid
        {"1.1.1.1", true},
        {"255.255.255.255", true},
        {"192.168.1.1", true},
        {"10.10.1.1", true},
        {"132.254.111.10", true},
        {"26.10.2.10", true},
        {"127.0.0.1", true},
        // invalid
        {"10.10.10", false},
        {"10.10", false},
        {"10", false},
        {"a.a.a.a", false},
        {"10.10.10.a", false},
        {"10.10.10.256", false},
        {"222.222.2.999", false},
        {"999.10.10.20", false},
        {"2222.22.22.22", false},
        {"22.2222.22.2", false},
        // IPv6
        {"2001:cdba:0000:0000:0000:0000:3257:9652", false},
        {"2001:cdba:0:0:0:0:3257:9652", false},
        {"2001:cdba::3257:9652", false},
    };
}

TEST_P(IpAddrSuite, CheckAddr)
{
    const auto& [addr, expected] = GetParam();

    ASSERT_THAT(util::is_ipaddr(addr), Eq(expected));
}


#if THREAD_SANITIZER
TEST(IpUtils, ThreadSafeGeneration)
{
    auto reader_thread = std::thread{[]{
        for (int i = 0; i < 8; ++i) {
            util::localhost();
        }
    }};

    auto updater_thread = std::thread{[]{
        for (int i = 0; i < 8; ++i) {
            util::update_localhost_addrs();
        }
    }};

    reader_thread.join();
    updater_thread.join();
}
#endif


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
