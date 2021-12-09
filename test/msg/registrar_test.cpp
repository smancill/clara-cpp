#include <clara/msg/topic.hpp>

#include "helper/registration.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace cm = clara::msg;
namespace t = clara::msg::test;

auto ctx = cm::detail::Context{};
auto driver = cm::detail::RegDriver{ctx, {}};

auto reg_data = cm::RegDataSet{};
auto name = std::string{"registrat_test"};


bool check_publisher(const cm::proto::Registration& reg)
{
    return reg.ownertype() == cm::proto::Registration::PUBLISHER;
}


cm::RegDataSet find(const std::string& topic, bool is_publisher)
{
    auto data = cm::RegDataSet{};
    auto search_topic = cm::Topic::raw(topic);
    for (auto& reg : reg_data) {
        if (is_publisher != check_publisher(reg)) {
            continue;
        }
        auto reg_topic = cm::Topic::build(reg.domain(), reg.subject(), reg.type());
        if (is_publisher) {
            if (search_topic.is_parent(reg_topic)) {
                data.insert(reg);
            }
        } else {
            if (reg_topic.is_parent(search_topic)) {
                data.insert(reg);
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
        driver.add(reg, check_publisher(reg));
        reg_data.insert(reg);
    }
}


void remove_random(int size)
{
    printf("INFO: Removing %d random actors...\n", size);

    auto gen = std::uniform_int_distribution<int>(0, reg_data.size() - size);
    auto first = gen(t::rng);
    auto end = first + size;
    auto i = 0;
    for (auto reg_it = reg_data.begin(); reg_it != reg_data.end(); ) {
        if (i == end) {
            break;
        }
        if (i >= first) {
            driver.remove(*reg_it, check_publisher(*reg_it));
            reg_it = reg_data.erase(reg_it);
        } else {
            ++reg_it;
        }
        ++i;
    }
}


void remove_host(const std::string& host)
{
    printf("INFO: Removing host %s\n", host.data());
    for (auto it = reg_data.begin(); it != reg_data.end(); ) {
        if (it->host() == host) {
            it = reg_data.erase(it);
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
    for (auto& host : t::hosts) {
        driver.remove_all("test", host);
    }
    reg_data.clear();
}


cm::proto::Registration discovery_request(const std::string& topic,
                                          bool is_publisher)
{
    return t::new_registration(name, "localhost", topic, is_publisher);
}


void check(bool is_publisher)
{
    for (auto& topic : t::topics) {
        auto data = discovery_request(topic, is_publisher);

        auto result = driver.find(data, is_publisher);
        auto expected = find(topic, is_publisher);

        if (result == expected) {
            auto owner = is_publisher ? "publishers" : "subscribers";
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
    check(true);
    check(false);
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
