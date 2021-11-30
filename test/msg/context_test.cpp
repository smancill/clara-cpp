#include <clara/msg/context.hpp>

#include <gmock/gmock.h>

#include <future>

namespace cm = clara::msg;

using namespace testing;


TEST(Context, SingletonSingleThreadInstances)
{
    auto ctx1 = cm::Context::instance();
    auto ctx2 = cm::Context::instance();

    ASSERT_THAT(ctx1, Eq(ctx2));
}


TEST(Context, SingletonMultiThreadInstances)
{
    auto l = []() { return cm::Context::instance(); };

    auto f1 = std::async(std::launch::async, l);
    auto f2 = std::async(std::launch::async, l);

    auto ctx1 = f1.get();
    auto ctx2 = f2.get();

    ASSERT_THAT(ctx1, Eq(ctx2));
}


TEST(Context, CreateNewContexts)
{
    auto ctx1 = cm::Context::create();
    auto ctx2 = cm::Context::create();

    ASSERT_THAT(ctx1.get(), Ne(ctx2.get()));
}


TEST(Context, SetIOThreads)
{
    auto ctx = cm::Context::create();

    ctx->set_io_threads(4);

    ASSERT_THAT(ctx->io_threads(), Eq(4));
}


TEST(Context, SetMaxSockets)
{
    auto ctx = cm::Context::create();

    ctx->set_max_sockets(2048);

    ASSERT_THAT(ctx->max_sockets(), Eq(2048));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
