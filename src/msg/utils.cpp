/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/utils.hpp>

#include <array>
#include <ctime>
#include <chrono>
#include <mutex>
#include <regex>
#include <shared_mutex>
#include <system_error>
#include <thread>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>


namespace {

// clang-format off
const auto ip_regex = std::regex{"^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
                                     "(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"};
// clang-format on


auto get_addresses() -> std::vector<std::string>
{
    struct AddrList {
        AddrList() { getifaddrs(&ifl); }

        ~AddrList() { if (ifl != nullptr) { freeifaddrs(ifl); } }

        struct ifaddrs* ifl = nullptr;
    } ifl;

    auto all_addrs = std::vector<std::string>{};

    for (struct ifaddrs* ifa = ifl.ifl; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) {
            void* addr_s = &reinterpret_cast<sockaddr_in*>(ifa->ifa_addr)->sin_addr;
            auto addr_b = std::array<char, INET_ADDRSTRLEN>{};
            inet_ntop(AF_INET, addr_s, addr_b.data(), addr_b.size());
            auto addr = std::string{addr_b.data()};
            if (addr.compare(0, 3, "127") != 0) {
                all_addrs.push_back(std::move(addr));
            }
        }
    }

    return all_addrs;
}


auto get_host_address(const std::string& hostname) -> std::string
{
    static auto mtx = std::mutex{};

    auto lock = std::lock_guard(mtx);

    struct hostent* h = gethostbyname(hostname.data());  // NOLINT(concurrency-mt-unsafe)
    if (h == nullptr) {
        throw std::system_error{EFAULT, std::system_category()};
    }
    return {inet_ntoa(*reinterpret_cast<in_addr*>(h->h_addr))};  // NOLINT(concurrency-mt-unsafe)
}


class LocalAddrs
{
public:
    void update_addrs()
    {
        auto updated = get_addresses();
        std::lock_guard<std::shared_timed_mutex> lk{mtx_};
        std::swap(addr_, updated);
    }

    auto get_first() -> std::string
    {
        std::shared_lock<std::shared_timed_mutex> lk{mtx_};
        return addr_[0];
    }

    auto get_addrs() -> std::vector<std::string>
    {
        std::shared_lock<std::shared_timed_mutex> lk{mtx_};
        return addr_;
    }

private:
    std::shared_timed_mutex mtx_;
    std::vector<std::string> addr_ = get_addresses();
};


auto local_addrs() -> LocalAddrs&
{
    static auto* addrs = new LocalAddrs{};
    return *addrs;
}


inline
auto safe_localtime(const std::time_t* time)
{
    static auto mtx = std::mutex{};

    auto lock = std::lock_guard(mtx);
    return std::localtime(time);  // NOLINT(concurrency-mt-unsafe)
}

}


namespace clara::msg::util {

auto localhost() -> std::string
{
    return local_addrs().get_first();
}


auto get_localhost_addrs() -> std::vector<std::string>
{
    return local_addrs().get_addrs();
}


void update_localhost_addrs()
{
    local_addrs().update_addrs();
}


auto to_host_addr(const std::string& hostname) -> std::string
{
    if (hostname == "localhost") {
        return local_addrs().get_first();
    }
    return get_host_address(hostname);
}


auto is_ipaddr(std::string_view hostname) -> bool
{
    return std::regex_match(hostname.cbegin(), hostname.cend(), ip_regex);
}


void sleep(long millis)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}


auto get_current_time() -> std::string
{
    auto now = std::time(nullptr);
    auto buf = std::array<char, sizeof "2001-01-01 00:00:00">{};
    std::strftime(buf.data(), buf.size(), "%Y-%m-%d %H:%M:%S", safe_localtime(&now));
    return {buf.begin(), buf.end()};
}

} // end namespace clara::msg::util
