/*
 * Copyright (C) 2015. Jefferson Lab, xMsg framework (JLAB). All Rights Reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research, and not-for-profit purposes,
 * without fee and without a signed licensing agreement.
 *
 * Contact Vardan Gyurjyan
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include "zhelper.hpp"

#include "likely.hpp"

#include <clara/msg/utils.hpp>

#include <atomic>
#include <mutex>
#include <random>
#include <sstream>
#include <tuple>

namespace {

template<typename T>
using IntDist = std::uniform_int_distribution<T>;

// the request IDs support overflow
using u32_t = std::uint_fast32_t;
using atomic_u32_t = std::atomic_uint_fast32_t;


class RandomGenerator
{
public:
    template<typename D>
    auto operator()(D&& dist)
    {
        std::lock_guard<std::mutex> guard{mtx_};
        return dist(rng_);
    }

private:
    std::mutex mtx_;

    std::mt19937_64 rng_{std::random_device{}()};
} rng;


// language identifier (Java:1, C++:2, Python:3)
constexpr auto cpp_id = 2;

constexpr auto rt_seq_max = 1'000'000;
constexpr auto rt_seq_base = cpp_id * rt_seq_max;
constexpr auto rt_seq_size = 7;

auto rt_seq = atomic_u32_t{rng(IntDist<u32_t>{0, rt_seq_max - 1})};

}


namespace clara::msg::detail {

// Read up to 3-part messages. Any message with more parts is unexpected and
// invalid.
RawMessage::RawMessage(zmq::socket_t& socket)
{
    while (true) {
        auto& msg = parts_[counter_];
        std::ignore = socket.recv(msg);
        ++counter_;
        if (counter_ == msg_size || !msg.more()) {
            break;
        }
    }

    if (CLARA_UNLIKELY(parts_.back().more())) {
        while (true) {
            zmq::message_t msg;
            std::ignore = socket.recv(msg);
            if (!msg.more()) {
                break;
            }
        }
        throw std::runtime_error{"Invalid multi-part message"};
    }
}


// replyTo generation: format is "ret:<id>:2[dddddd]"
auto get_unique_replyto(const std::string& subject) -> std::string
{
    constexpr auto rt_fixed_size = 4 + 1 + rt_seq_size;

    auto id = ++rt_seq % rt_seq_max + rt_seq_base;

    auto rt = std::string{};
    rt.reserve(rt_fixed_size + subject.size());
    rt.append("ret:").append(subject).append(":").append(std::to_string(id));
    return rt;
}


void set_unique_replyto(std::uint_fast32_t value)
{
    rt_seq = value;
}


// actor unique ID: format is 8 digits: [dddddddd]
auto encode_identity(const std::string& address, const std::string& name)
    -> std::string
{
    constexpr auto id_size = 8;
    constexpr auto max_suffix = 99;

    static auto dist = IntDist<std::int_fast8_t>{0, max_suffix};

    auto id = address + "#" + name + "#" + std::to_string(rng(dist));
    auto ss = std::stringstream{};
    ss << std::hex << std::hash<std::string>{}(id);
    return ss.str().substr(0, id_size);
}


// control ID generation: format is 9 digits: 2[ppp][ddddd]
auto get_random_id() -> std::string
{
    static const auto id_prefix = []{
        auto ip_hash = std::hash<std::string>{}(util::localhost());
        return cpp_id * 100'000'000L + (ip_hash % 1'000) * 100'000;
    }();

    static auto id_dist = IntDist<std::int_fast32_t>{0, 99'999};

    return std::to_string(id_prefix + rng(id_dist));
}

} // end namespace clara::msg::detail
