#ifndef CLARA_MSG_TEST_HELPER_REG_H_
#define CLARA_MSG_TEST_HELPER_REG_H_

#include "registration_driver.hpp"

#include <clara/msg/address.hpp>
#include <clara/msg/topic.hpp>

#include <random>
#include <string>

namespace clara::msg::test {

using DataArray = std::vector<std::string>;

// clang-format off
const DataArray topics = {
    "writer",
    "writer:adventures",
    "writer:adventures:books",
    "writer:adventures:tales",
    "writer:scifi:books",
    "writer:scifi:tales",
    "actor",
    "actor:action",
    "actor:drama",
    "actor:comedy",
    "actor:action:movies",
    "actor:action:series",
    "actor:comedy:movies",
    "actor:comedy:series",
    "actor:drama:movies",
    "actor:drama:series",
};

const DataArray names = {
    "A", "B", "C", "D", "E",
    "F", "G", "H", "I", "J",
    "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y",
    "Z"};

const DataArray hosts = {
    "10.2.9.50", "10.2.9.60",
    "10.2.9.51", "10.2.9.61",
    "10.2.9.52", "10.2.9.62",
    "10.2.9.53", "10.2.9.63",
    "10.2.9.54", "10.2.9.64",
    "10.2.9.55", "10.2.9.65",
    "10.2.9.56", "10.2.9.66",
    "10.2.9.57", "10.2.9.67",
    "10.2.9.58", "10.2.9.68",
    "10.2.9.59", "10.2.9.69",
};
// clang-format on


auto new_registration(std::string_view name,
                      std::string_view host,
                      std::string_view topic,
                      bool is_publisher) -> proto::Registration
{
    return registration::create(name, "test data",
                                host, ProxyAddress::default_port,
                                Topic::raw(topic), is_publisher);
}


template<typename D>
auto next(D& dist) -> typename D::result_type
{
    static auto rng = std::mt19937{std::random_device{}()};

    return dist(rng);
}


auto random(const DataArray& data) -> const std::string&
{
    static auto udist = std::uniform_int_distribution<std::size_t>{
        0, topics.size() * names.size() * hosts.size()
    };

    auto idx = next(udist) % data.size();
    return data[idx];
}


auto random_registration() -> proto::Registration
{
    static auto bdist = std::bernoulli_distribution{};

    const auto& name = random(names);
    const auto& host = random(hosts);
    const auto& topic = random(topics);
    auto is_publisher = next(bdist);
    return new_registration(name, host, topic, is_publisher);
}

} // end namespace clara::msg::test

#endif
