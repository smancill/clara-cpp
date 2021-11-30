#include <clara/msg/actor.hpp>
#include <clara/msg/utils.hpp>

#include "helper/proxy_wrapper.hpp"
#include "helper/utils.hpp"

#include <gmock/gmock.h>

#include <atomic>

namespace cm = clara::msg;

using namespace testing;


TEST(Subscription, UnsubscribeStopsThread)
{
    cm::test::ProxyThread proxy_thread{};

    auto actor = cm::Actor{"test"};
    auto con = actor.connect();

    auto cb = [](cm::Message&) {};
    auto sub = actor.subscribe(cm::Topic::build("test"), std::move(con), cb);
    cm::util::sleep(1000);

    actor.unsubscribe(std::move(sub));
}


TEST(Subscription, SuscribeReceivesAllMessages)
{
    struct Check
    {
        std::atomic_int counter{0};
        std::atomic_long sum{0};

        const int N = 10000;
        const long SUM_N = 49995000L;
    } check;

    cm::test::ProxyThread proxy_thread;

    SimpleCondition sub_ready;
    SimpleCondition all_msg;

    auto sub_thread = std::thread{[&]() {
        try {
            auto actor = cm::Actor{"test_subscriber"};
            auto connection = actor.connect();

            auto topic = cm::Topic::raw("test_topic");
            auto cb = [&](cm::Message& msg) {
                auto i = cm::parse_message<int>(msg);
                auto n = ++check.counter;
                check.sum += i;
                if (n == check.N) {
                    all_msg.notify_one();
                }
            };

            auto sub = actor.subscribe(topic, std::move(connection), cb);
            sub_ready.notify_one();

            all_msg.wait_for(4000);
            actor.unsubscribe(std::move(sub));
        } catch (std::exception& e) {
            std::cerr << "Subscriber error: " << e.what() << std::endl;
        }
    }};

    auto pub_thread = std::thread{[&]() {
        try {
            sub_ready.wait_for(1000);
            auto actor = cm::Actor{"test_publisher"};
            auto connection = actor.connect();
            auto topic = cm::Topic::raw("test_topic");
            for (int i = 0; i < check.N; i++) {
                auto msg = cm::make_message(topic, i);
                actor.publish(connection, msg);
            }
        } catch (std::exception& e) {
            std::cerr << "Publisher error: " << e.what() << std::endl;
        }
    }};

    pub_thread.join();
    sub_thread.join();

    ASSERT_THAT(check.counter.load(), Eq(check.N));
    ASSERT_THAT(check.sum.load(), Eq(check.SUM_N));
}


TEST(Subscription, SyncPublishReceivesAllResponses)
{
    struct Check
    {
        std::atomic_int counter{0};
        std::atomic_long sum{0};

        const int N = 100;
        const long SUM_N = 4950L;
    } check;

    cm::test::ProxyThread proxy_thread;

    SimpleCondition sub_ready;
    SimpleCondition all_msg;

    auto sub_thread = std::thread{[&]() {
        try {
            auto sub_actor = cm::Actor{"test_subscriber"};
            auto sub_con = sub_actor.connect();
            auto rep_con = sub_actor.connect();

            auto sub_topic = cm::Topic::raw("test_topic");
            auto sub_cb = [&](cm::Message& m) {
                auto r_topic = m.meta()->replyto();
                auto r_data = cm::parse_message<int>(m);
                auto r_msg = cm::make_message(cm::Topic::raw(r_topic), r_data);
                sub_actor.publish(rep_con, r_msg);
            };

            auto sub = sub_actor.subscribe(sub_topic, std::move(sub_con), sub_cb);
            sub_ready.notify_one();

            all_msg.wait_for(8000);
            sub_actor.unsubscribe(std::move(sub));
        } catch (std::exception& e) {
            std::cerr << "Subscriber error: " << e.what() << std::endl;
        }
    }};

    auto pub_thread = std::thread{[&]() {
        try {
            sub_ready.wait_for(1000);
            auto pub_actor = cm::Actor{"test_publisher"};
            auto pub_con = pub_actor.connect();
            auto pub_topic = cm::Topic::raw("test_topic");
            for (int i = 0; i < check.N; i++) {
                auto msg = cm::make_message(pub_topic, i);
                auto r_msg = pub_actor.sync_publish(pub_con, msg, 1000);
                auto r_data = cm::parse_message<int>(r_msg);
                ++check.counter;
                check.sum += r_data;
            }
        } catch (std::exception& e) {
            std::cerr << "Publisher error: " << e.what() << std::endl;
        }
    }};

    pub_thread.join();
    all_msg.notify_one();
    sub_thread.join();

    ASSERT_THAT(check.counter.load(), Eq(check.N));
    ASSERT_THAT(check.sum.load(), Eq(check.SUM_N));
}


TEST(MultiThreadPublisher, SuscribeReceivesAllMessages)
{
    struct Check
    {
        std::atomic_int counter{0};
        std::atomic_long sum{0};

        const int THREADS = 4;
        const int N = 10000;
        const long SUM_N = 49995000L;
    } check;

    cm::test::ProxyThread proxy_thread;

    SimpleCondition sub_ready;
    SimpleCondition all_msg;

    auto sub_thread = std::thread{[&]() {
        try {
            auto actor = cm::Actor{"test_subscriber"};
            auto connection = actor.connect();

            auto topic = cm::Topic::raw("test_topic");
            auto cb = [&](cm::Message& msg) {
                auto i = cm::parse_message<int>(msg);
                auto n = ++check.counter;
                check.sum += i;
                if (n == check.N) {
                    all_msg.notify_one();
                }
            };

            auto sub = actor.subscribe(topic, std::move(connection), cb);
            sub_ready.notify_all();

            all_msg.wait_for(4000);
            actor.unsubscribe(std::move(sub));
        } catch (std::exception& e) {
            std::cerr << "Subscriber error: " << e.what() << std::endl;
        }
    }};

    auto pub_actor = cm::Actor{"test_publisher"};
    auto pub_threads = std::vector<std::thread>{};
    for (int i = 0; i < check.THREADS; ++i) {
        pub_threads.emplace_back([&,i]() {
            try {
                sub_ready.wait_for(1000);
                auto n = check.N / check.THREADS;
                auto topic = cm::Topic::raw("test_topic");
                for (int j = n * i; j < n * (i + 1); j++) {
                    auto connection = pub_actor.connect();
                    auto msg = cm::make_message(topic, j);
                    pub_actor.publish(connection, msg);
                }
            } catch (std::exception& e) {
                std::cerr << "Publisher " << i << " error: " << e.what()
                          << std::endl;
            }
        });
    }

    for (auto& t : pub_threads) {
        t.join();
    }
    sub_thread.join();

    ASSERT_THAT(check.counter.load(), Eq(check.N));
    ASSERT_THAT(check.sum.load(), Eq(check.SUM_N));
}


TEST(MultiThreadPublisher, SyncPublishReceivesAllResponses)
{
    struct Check
    {
        std::atomic_int counter{0};
        std::atomic_long sum{0};

        const int THREADS = 4;
        const int N = 100;
        const long SUM_N = 4950L;
    } check;

    cm::test::ProxyThread proxy_thread{};

    SimpleCondition sub_ready;
    SimpleCondition all_msg;

    auto sub_thread = std::thread{[&]() {
        try {
            auto sub_actor = cm::Actor{"test_subscriber"};
            auto sub_con = sub_actor.connect();
            auto rep_con = sub_actor.connect();

            auto topic = cm::Topic::raw("test_topic");
            auto cb = [&](cm::Message& msg) {
                auto r_topic = msg.meta()->replyto();
                auto r_data = cm::parse_message<int>(msg);
                auto r_msg = cm::make_message(cm::Topic::raw(r_topic), r_data);
                sub_actor.publish(rep_con, r_msg);
            };

            auto sub = sub_actor.subscribe(topic, std::move(sub_con), cb);
            sub_ready.notify_all();

            all_msg.wait_for(8000);
            sub_actor.unsubscribe(std::move(sub));
        } catch (std::exception& e) {
            std::cerr << "Subscriber error: " << e.what() << std::endl;
        }
    }};

    auto pub_actor = cm::Actor{"test_publisher"};
    auto pub_threads = std::vector<std::thread>{};
    for (int i = 0; i < check.THREADS; ++i) {
        pub_threads.emplace_back([&,i]() {
            try {
                sub_ready.wait_for(1000);
                auto topic = cm::Topic::raw("test_topic");
                auto n = check.N / check.THREADS;
                for (int j = n * i; j < n * (i + 1); j++) {
                    auto con = pub_actor.connect();
                    auto msg = cm::make_message(topic, j);
                    auto r_msg = pub_actor.sync_publish(con, msg, 1000);
                    auto r_data = cm::parse_message<int>(r_msg);
                    ++check.counter;
                    check.sum += r_data;
                }
            } catch (std::exception& e) {
                std::cerr << "Publisher " << i << " error: " << e.what()
                          << std::endl;
            }
        });
    }

    for (auto& t : pub_threads) {
        t.join();
    }
    all_msg.notify_one();
    sub_thread.join();

    ASSERT_THAT(check.counter.load(), Eq(check.N));
    ASSERT_THAT(check.sum.load(), Eq(check.SUM_N));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
