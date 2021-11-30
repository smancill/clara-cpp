/*
 * Copyright (c) 2016.  Jefferson Lab (JLab). All rights reserved. Permission
 * to use, copy, modify, and distribute  this software and its documentation for
 * educational, research, and not-for-profit purposes, without fee and without a
 * signed licensing agreement.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY,
 * PROVIDED HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * This software was developed under the United States Government license.
 * For more information contact author at gurjyan@jlab.org
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 */

#ifndef CLARA_DPE_OPTIONS_HPP
#define CLARA_DPE_OPTIONS_HPP

#include "constants.hpp"
#include "dpe_config.hpp"

#include <clara/msg/address.hpp>

#include <cxxopts.hpp>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace clara {

namespace opt {

constexpr auto host = "host";
constexpr auto port = "port";
constexpr auto fe_host = "fe-host";
constexpr auto fe_port = "fe-port";
constexpr auto session = "session";
constexpr auto description = "description";
constexpr auto poolsize = "poolsize";
constexpr auto max_cores = "max-cores";
constexpr auto report = "report";
constexpr auto max_sockets = "max-sockets";
constexpr auto io_threads = "io-threads";

}


class DpeOptionsParser final
{
public:
    DpeOptionsParser()
    {
        using cxxopts::value;

        options_.add_options("main")
            (opt::host, "use given host for this DPE", value<std::string>())
            (opt::port, "use given port for this DPE", value<int>())
            (opt::fe_host, "the host used by the remote front-end", value<std::string>())
            (opt::fe_port, "the port used by the remote front-end", value<int>())
            ;

        options_.add_options("config")
            (opt::session, "the session ID of this DPE", value<std::string>())
            (opt::description, "a short description of this DPE", value<std::string>())
            (opt::poolsize, "size of thread pool to handle requests", value<int>())
            (opt::max_cores, "how many cores can be used by a service", value<int>())
            (opt::report, "the period to publish reports [s]", value<int>())
            ;

        options_.add_options("advanced")
            (opt::max_sockets, "maximum number of allowed ZMQ sockets", value<int>())
            (opt::io_threads, "size of ZMQ thread pool to handle I/O", value<int>())
            ;

        options_.add_options("other")
            ("h,help", "Print usage");

        options_.set_width(88);
    }

    auto parse(int argc, char* argv[]) -> bool  // NOLINT
    try {
        using namespace std::literals::string_literals;

        result_ = options_.parse(argc, argv);

        if (has_help()) {
            return true;
        }

        // C++ DPEs are alwåys worker DPEs that require a FE argument
        if (!has_frontend()) {
            return false;
        }

        // Get local DPE address
        auto local_host = get(opt::host, "localhost"s);
        auto local_port = get(opt::port, constants::cpp_port);
        local_addr_ = msg::ProxyAddress{local_host, local_port};

        // Get remote FE address
        auto fe_host = result_[opt::fe_host].as<std::string>();
        auto fe_port = get(opt::fe_port, constants::java_port);
        fe_addr_ = msg::ProxyAddress{fe_host, fe_port};

        // Get config options
        config_ = {
            get(opt::session, ""s),
            get(opt::description, ""s),
            get(opt::poolsize, DpeConfig::default_pool_size),
            get(opt::max_cores, DpeConfig::default_max_cores),
            parse_report_period()
        };

        // Get ZMQ options
        max_sockets_ = get(opt::max_sockets, 1024);
        io_threads_ = get(opt::io_threads, 1);

        return true;
    } catch (const cxxopts::OptionException& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
    }

    auto has_help() -> bool
    {
        return result_.count("help") > 0;
    }

    void print_help()
    {
        std::cout << options_.help({"main", "config", "advanced", "other"})
                  << std::endl;
    }

private:
    auto has_frontend() -> bool
    {
        if (result_.count(opt::fe_host) == 0) {
            if (result_.count(opt::fe_port) == 0) {
                std::cerr << "The remote front-end host is required." << std::endl;
            } else {
                std::cerr << "Missing front-end host argument." << std::endl;
            }
            return false;
        }
        return true;
    }

    template<typename T>
    auto get(const std::string& opt, const T& default_value) -> T
    {
        return (result_.count(opt) > 0) ? result_[opt].as<T>() : default_value;
    }

    auto parse_report_period() -> int
    {
        using namespace std::chrono;
        auto ms = milliseconds{DpeConfig::default_report_period};
        auto s = get(opt::report, int(duration_cast<seconds>(ms).count()));
        return duration_cast<milliseconds>(seconds{s}).count();
    }

public:
    auto local_address() const -> msg::ProxyAddress
    {
        return local_addr_;
    }

    auto frontend_address() const -> msg::ProxyAddress
    {
        return fe_addr_;
    }

    auto config() const -> DpeConfig
    {
        return config_;
    }

    auto max_sockets() const -> int
    {
        return max_sockets_;
    }

    auto io_threads() const -> int
    {
        return io_threads_;
    }

private:
    cxxopts::Options options_{"c_dpe", "CLARA C++ DPE\n"};
    cxxopts::ParseResult result_;

    msg::ProxyAddress local_addr_;
    msg::ProxyAddress fe_addr_;

    DpeConfig config_;

    int max_sockets_;
    int io_threads_;
};

} // end namespace clara

#endif // end of include guard: CLARA_DPE_OPTIONS_HPP
