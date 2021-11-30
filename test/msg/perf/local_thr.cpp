#include <clara/msg/actor.hpp>
#include <clara/msg/utils.hpp>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <future>
#include <iostream>
#include <string>

namespace cm = clara::msg;

auto elapsed_time_promise = std::promise<double>{};


class LocalCallback
{
public:
    LocalCallback(int msg_size, int msg_count)
      : msg_size{msg_size}
      , msg_count{msg_count}
    { }

    void operator()(cm::Message& msg);

private:
    using clock = std::chrono::high_resolution_clock;
    using point = decltype(clock::now());
    using us = std::chrono::microseconds;

    void start_clock();
    auto stop_clock() -> long;

private:
    const int msg_size;
    const int msg_count;

    point watch;
    int nr = 0;
};


int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cerr << "usage: local_thr <bind-to> <message-size> <message-count>" << std::endl;
        return EXIT_FAILURE;
    }

    const auto bind_to = cm::util::to_host_addr(argv[1]);
    const auto message_size = std::stoi(argv[2]);
    const auto message_count = std::stoi(argv[3]);

    try {
        auto subscriber = cm::Actor("thr_subscriber");
        auto connection = subscriber.connect(cm::ProxyAddress{bind_to});
        auto topic = cm::Topic::raw("thr_topic");
        auto cb = LocalCallback{message_size, message_count};

        auto sub = subscriber.subscribe(topic, std::move(connection), cb);
        std::cout << "Waiting for messages..." << std::endl;

        const double elapsed_time = elapsed_time_promise.get_future().get();
        const double throughput = message_count / (elapsed_time / 1'000'000);
        const double megabits = (throughput * message_size * 8) / 1'000'000;
        const double latency = elapsed_time / message_count;

        printf("message elapsed: %.3f [s]\n", elapsed_time / 1'000'000);
        printf("message size: %d [B]\n", message_size);
        printf("message count: %d\n", message_count);
        printf("mean transfer time: %.3f [us]\n", latency);
        printf("mean transfer rate: %d [msg/s]\n", static_cast<int>(throughput));
        printf("mean throughput: %.3f [Mb/s]\n", megabits);
        printf("mean throughput: %.3f [MByte/s]\n", megabits / 8);

        subscriber.unsubscribe(std::move(sub));

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void LocalCallback::operator()(cm::Message& msg)
{
    auto size = static_cast<int>(msg.data().size());
    if (size != msg_size) {
        std::cerr << "message of incorrect size received: " << size << std::endl;
        std::abort();
    }
    ++nr;
    if (nr == 1) {
        start_clock();
    } else if (nr == msg_count) {
        auto dt = stop_clock();
        elapsed_time_promise.set_value(static_cast<double>(dt));
    }
}


void LocalCallback::start_clock()
{
    watch = clock::now();
}


auto LocalCallback::stop_clock() -> long
{
    return std::chrono::duration_cast<us>(clock::now() - watch).count();
}
