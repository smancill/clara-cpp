/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/topic.hpp>

#include "helper/registration.hpp"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace cm = clara::msg;
namespace t = clara::msg::test;

using Type = cm::proto::Registration::Type;

constexpr auto PUBLISHER = cm::proto::Registration::PUBLISHER;
constexpr auto SUBSCRIBER = cm::proto::Registration::SUBSCRIBER;

auto ctx = cm::detail::Context{};
auto driver = cm::detail::RegDriver{ctx, {}};

auto registration = cm::RegDataSet{};
auto name = std::string{"registrat_test"};


auto check_publisher(Type type) -> bool
{
    return type == PUBLISHER;
}


auto find(std::string_view topic, Type type) -> cm::RegDataSet
{
    auto data = cm::RegDataSet{};
    auto search_topic = cm::Topic::raw(topic);
    for (const auto& reg : registration) {
        if (reg.type() != type) {
            continue;
        }
        auto reg_topic = cm::Topic::raw(reg.topic());
        switch (type) {
            case PUBLISHER: {
                if (search_topic.is_parent(reg_topic)) {
                    data.insert(reg);
                }
                break;
            }
            case SUBSCRIBER: {
                if (reg_topic.is_parent(search_topic)) {
                    data.insert(reg);
                }
                break;
            }
        }
    }
    return data;
}


void add_random(int size)
{
    printf("INFO: Registering %d random actors...\n", size);
    for (int i = 0; i < size; i++) {
        auto reg = t::random_registration();
        driver.add(name, reg);
        registration.insert(reg);
    }
}


void remove_random(int size)
{
    printf("INFO: Removing %d random actors...\n", size);

    auto gen = std::uniform_int_distribution<std::size_t>(0, registration.size() - size);
    auto first = t::next(gen);
    auto end = first + size;
    auto j = 0U;
    for (auto it = registration.begin(); it != registration.end(); ) {
        if (j == end) {
            break;
        }
        if (j >= first) {
            driver.remove(name, *it);
            it = registration.erase(it);
        } else {
            ++it;
        }
        ++j;
    }
}


void remove_host(std::string_view host)
{
    printf("INFO: Removing host %s\n", host.data());
    for (auto it = registration.begin(); it != registration.end(); ) {
        if (it->host() == host) {
            it = registration.erase(it);
        } else {
            ++it;
        }
    }
    driver.remove_all("test", host);
}


void remove_random_host()
{
    remove_host(t::random(t::hosts));
}


void remove_all()
{
    printf("INFO: Removing all actors\n");
    for (const auto& host : t::hosts) {
        driver.remove_all("test", host);
    }
    registration.clear();
}


auto discovery_request(std::string_view topic, Type type)
    -> cm::proto::Registration
{
    return t::new_reg_filter(type, topic);
}


void check(Type type)
{
    for (const auto& topic : t::topics) {
        auto data = discovery_request(topic, type);

        auto result = driver.find(name, data);
        auto expected = find(topic, type);

        if (result == expected) {
            auto owner = type == PUBLISHER ? "publishers" : "subscribers";
            printf("Found %3zd %s for %s\n", result.size(), owner, topic.data());
        } else {
            printf("Topic: %s\n", topic.data());
            printf("Result: %zd\n", result.size());
            printf("Expected: %zd\n", expected.size());
            printf("Sets doesn't match!!!\n");
            exit(EXIT_FAILURE);
        }
    }
}


void check()
{
    check(PUBLISHER);
    check(SUBSCRIBER);
}


void test_registration_database()
{
    add_random(10000);
    check();

    remove_random(2500);
    check();

    add_random(1000);
    check();

    remove_random_host();
    check();

    add_random(1000);
    check();

    remove_all();
    check();
}


int main()
{
    using Time = std::chrono::high_resolution_clock;
    using s = std::chrono::duration<float>;

    auto timer = [t = Time::now()]() {
        return std::chrono::duration_cast<s>(Time::now() - t).count();
    };

    test_registration_database();

    printf("Total time: %.2f [s]\n", timer());
    return 0;
}
