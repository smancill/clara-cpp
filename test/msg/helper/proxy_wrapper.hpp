#ifndef CLARA_MSG_TEST_PROXY_WRAPPER_H_
#define CLARA_MSG_TEST_PROXY_WRAPPER_H_

#include <clara/msg/constants.hpp>
#include <clara/msg/proxy.hpp>
#include <clara/msg/utils.hpp>

#include <iostream>
#include <string>
#include <thread>

namespace xmsg {

namespace test {

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
        xmsg::util::sleep(100);
    }

    ~ProxyThread()
    {
        proxy_.stop();
    }

private:
    xmsg::sys::Proxy proxy_{{}};
};

} // end namespace test
} // end namespace xmsg

#endif
