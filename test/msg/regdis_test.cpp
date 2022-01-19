/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "helper/registration.hpp"

#include "constants.hpp"

#include <gmock/gmock.h>

#include <iostream>

namespace cm = clara::msg;
namespace cc = clara::msg::constants;
namespace t = clara::msg::test;

using namespace testing;

using Request = cm::detail::Request;
using Response = cm::detail::Response;


constexpr auto PUBLISHER = cm::proto::Registration::PUBLISHER;
constexpr auto SUBSCRIBER = cm::proto::Registration::SUBSCRIBER;


template<typename R>
auto move_msg(R& r) -> R
{
    return R{std::move(r.msg())};
}


TEST(Request, CreateDataRequest)
{
    auto data = t::new_reg_data("asimov", "10.2.9.1", "writer:scifi", PUBLISHER);

    auto send_req = Request{"reg_action", "test_actor", data};
    auto recv_req = move_msg(send_req);

    EXPECT_THAT(recv_req.action(), StrEq("reg_action"));
    EXPECT_THAT(recv_req.sender(), StrEq("test_actor"));
    EXPECT_THAT(recv_req.data(), Eq(data));
}


TEST(Response, CreateSuccessResponse)
{
    auto send_res = Response{"reg_action", "reg_fe"};
    auto recv_res = move_msg(send_res);

    EXPECT_THAT(recv_res.action(), StrEq("reg_action"));
    EXPECT_THAT(recv_res.sender(), StrEq("reg_fe"));
    EXPECT_THAT(recv_res.status(), StrEq(cc::success));
    EXPECT_THAT(recv_res.data(), SizeIs(0));
}


TEST(Response, CreateErrorResponse)
{
    auto error = "could not handle request";
    auto send_res = Response{"reg_action", "reg_fe", error};

    EXPECT_THAT(send_res.action(), StrEq("reg_action"));
    EXPECT_THAT(send_res.sender(), StrEq("reg_fe"));
    EXPECT_THAT(send_res.status(), StrEq(error));
    EXPECT_THAT(send_res.data(), SizeIs(0));
}


TEST(Response, CreateDataResponse)
{
    auto data = cm::RegDataSet{
        t::new_reg_data("asimov", "10.2.9.1", "writer.scifi:books", PUBLISHER),
        t::new_reg_data("bradbury", "10.2.9.1", "writer.scifi:books", SUBSCRIBER),
    };

    auto send_res = Response{"reg_action", "reg_fe", data};
    auto recv_res = move_msg(send_res);

    EXPECT_THAT(recv_res.action(), StrEq("reg_action"));
    EXPECT_THAT(recv_res.sender(), StrEq("reg_fe"));
    EXPECT_THAT(recv_res.status(), StrEq(cc::success));
    EXPECT_THAT(recv_res.data(), ContainerEq(data));
}


class MockContext : public cm::detail::Context { };


class MockDriver : public cm::detail::RegDriver
{
    friend struct DriverTest;

    MockDriver(MockContext& ctx)
      : cm::detail::RegDriver{ctx, {}} {};

    MOCK_METHOD2(request, Response(Request&, int));
};

struct DriverTest : public Test
{
    DriverTest()
      : driver{ctx}
      , subscriber{t::new_reg_data("bradbury_sub", "localhost",
                                   "writer:scifi:books", SUBSCRIBER)}
      , publisher{t::new_reg_data("bradbury_pub", "localhost",
                                  "writer:scifi:books", PUBLISHER)}
    {
        ON_CALL(driver, request(_, _)).WillByDefault([]() {
            return Response{"", ""};
        });
    }

    void set_response(const cm::RegDataSet& data)
    {
        EXPECT_CALL(driver, request(_, _)).WillOnce([&]() {
            return Response{"", "", data};
        });
    }

    void expect_request(const Request& reg, int timeout)
    {
        EXPECT_CALL(driver, request(Eq(std::ref(reg)), Eq(timeout)));
    }

    static constexpr auto sender = std::string_view{"test_sender"};

    MockContext ctx;
    MockDriver driver;

    cm::proto::Registration subscriber;
    cm::proto::Registration publisher;
};


TEST_F(DriverTest, SendPublisherRegistration)
{
    auto req = Request(cc::reg_add, "test_sender", publisher);

    expect_request(req, cc::reg_add_timeout);

    driver.add("test_sender", publisher);
}


TEST_F(DriverTest, SendSubscriberRegistration)
{
    auto req = Request(cc::reg_add, "test_sender", subscriber);

    expect_request(req, cc::reg_add_timeout);

    driver.add("test_sender", subscriber);
}


TEST_F(DriverTest, SendPublisherRemoval)
{
    auto req = Request(cc::reg_remove, "test_sender", publisher);

    expect_request(req, cc::reg_remove_timeout);

    driver.remove("test_sender", publisher);
}


TEST_F(DriverTest, SendSubscriberRemoval)
{
    auto req = Request(cc::reg_remove, "test_sender", subscriber);

    expect_request(req, cc::reg_remove_timeout);

    driver.remove("test_sender", subscriber);
}


TEST_F(DriverTest, SendHostRemoval)
{
    auto pub_data = t::new_reg_filter(PUBLISHER);
    auto sub_data = t::new_reg_filter(SUBSCRIBER);

    pub_data.set_host("10.2.9.1");
    sub_data.set_host("10.2.9.1");

    auto pub_req = Request(cc::reg_remove_all, "test_sender", pub_data);
    auto sub_req = Request(cc::reg_remove_all, "test_sender", sub_data);

    expect_request(pub_req, cc::reg_remove_timeout);
    expect_request(sub_req, cc::reg_remove_timeout);

    driver.remove_all("test_sender", "10.2.9.1");
}


TEST_F(DriverTest, SendPublisherFind)
{
    auto data = t::new_reg_filter(PUBLISHER, "bradbury:scifi:books");
    auto req = Request(cc::reg_find_matching, "test_sender", data);

    expect_request(req, cc::reg_find_timeout);

    driver.find("test_sender", data);
}


TEST_F(DriverTest, SendSubscriberFind)
{
    auto data = t::new_reg_filter(SUBSCRIBER, "bradbury:scifi:books");
    auto req = Request(cc::reg_find_matching, "test_sender", data);

    expect_request(req, cc::reg_find_timeout);

    driver.find("test_sender", data);
}


TEST_F(DriverTest, GetRegistration)
{
    auto data = t::new_reg_filter(SUBSCRIBER);
    auto result = cm::RegDataSet{publisher, subscriber};

    set_response(result);

    auto res = driver.find(sender, data);

    EXPECT_THAT(res, ContainerEq(result));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
