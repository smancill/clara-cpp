/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_DPE_HPP
#define CLARA_DPE_HPP

#include <memory>

namespace clara {

namespace msg {
    class ProxyAddress;
} // end namespace msg

struct DpeConfig;


class Dpe final
{
public:
    Dpe(bool is_frontend,
        msg::ProxyAddress local,
        msg::ProxyAddress frontend,
        DpeConfig config);

    ~Dpe();

public:
    void start();

    void stop();

private:
    class DpeImpl;
    std::unique_ptr<DpeImpl> dpe_;
};

} // end namespace clara

#endif // end of include guard: CLARA_DPE_HPP
