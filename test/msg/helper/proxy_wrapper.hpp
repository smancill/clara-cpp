/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_TEST_PROXY_WRAPPER_H_
#define CLARA_MSG_TEST_PROXY_WRAPPER_H_

#include <clara/msg/proxy.hpp>
#include <clara/msg/utils.hpp>

#include <iostream>
#include <string>
#include <thread>

namespace clara::msg::test {

class ProxyThread
{
public:
    ProxyThread()
    {
        try {
            proxy_.start();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        util::sleep(100);
    }

    ~ProxyThread()
    {
        proxy_.stop();
    }

private:
    sys::Proxy proxy_{{}};
};

} // end namespace clara::msg::test

#endif
