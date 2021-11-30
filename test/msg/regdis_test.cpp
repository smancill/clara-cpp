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


TEST(Request, CreateDataRequest)
{
    auto data = t::new_registration("asimov", "10.2.9.1", "writer:scifi", true);

    auto send_req = Request{"foo:bar", "foo_service", data};
    auto recv_req = Request{send_req.msg()};

    EXPECT_THAT(recv_req.topic(), StrEq("foo:bar"));
    EXPECT_THAT(recv_req.sender(), StrEq("foo_service"));
    EXPECT_THAT(recv_req.data(), Eq(data));
}

TEST(Request, CreateTextRequest)
{
    auto send_req = Request{"foo:bar", "foo_service", "10.2.9.2"};
    auto recv_req = Request{send_req.msg()};

    EXPECT_THAT(recv_req.topic(), StrEq("foo:bar"));
    EXPECT_THAT(recv_req.sender(), StrEq("foo_service"));
    EXPECT_THAT(recv_req.text(), StrEq("10.2.9.2"));
}



TEST(Response, CreateSuccessResponse)
{
    auto send_res = Response{"foo:bar", "registration_fe"};
    auto recv_res = Response{send_res.msg()};

    EXPECT_THAT(recv_res.topic(), StrEq("foo:bar"));
    EXPECT_THAT(recv_res.sender(), StrEq("registration_fe"));
    EXPECT_THAT(recv_res.status(), StrEq(cc::success));
    EXPECT_THAT(recv_res.data(), SizeIs(0));
}

TEST(Response, CreateErrorResponse)
{
    auto error = "could not handle request";
    auto send_res = Response{"foo:bar", "registration_fe", error};

    EXPECT_THAT(send_res.topic(), StrEq("foo:bar"));
    EXPECT_THAT(send_res.sender(), StrEq("registration_fe"));
    EXPECT_THAT(send_res.status(), StrEq(error));
    EXPECT_THAT(send_res.data(), SizeIs(0));
}

TEST(Response, CreateDataResponse)
{
    auto data = cm::RegDataSet{
        t::new_registration("asimov", "10.2.9.1", "writer.scifi:books", true),
        t::new_registration("bradbury", "10.2.9.1", "writer.scifi:books", true),
    };

    auto send_res = Response{"foo:bar", "registration_fe", data};
    auto recv_res = Response{send_res.msg()};

    EXPECT_THAT(recv_res.topic(), StrEq("foo:bar"));
    EXPECT_THAT(recv_res.sender(), StrEq("registration_fe"));
    EXPECT_THAT(recv_res.status(), StrEq(cc::success));
    EXPECT_THAT(recv_res.data(), ContainerEq(data));
}



class MockContext : public cm::detail::Context
{
};

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
      , subscriber{t::new_registration("bradbury_sub", "localhost",
                                       "writer:scifi:books", false)}
      , publisher{t::new_registration("bradbury_pub", "localhost",
                                      "writer:scifi:books", true)}
    {
        ON_CALL(driver, request(_, _))
            .WillByDefault(Return(Response{"", "", ""}));
    }

    void set_response(const Response& response)
    {
        EXPECT_CALL(driver, request(_, _)).WillOnce(Return(response));
    }

    template<typename T>
    void expect_request(const std::string& name,
                        T&& data,
                        const std::string& topic,
                        int timeout)
    {
        auto reg = Request{topic, name, std::forward<T>(data)};
        EXPECT_CALL(driver, request(reg, timeout));
    }

    MockContext ctx;
    MockDriver driver;
    cm::proto::Registration subscriber;
    cm::proto::Registration publisher;
};


TEST_F(DriverTest, SendPublisherRegistration)
{
    expect_request("bradbury_pub",
                   publisher,
                   cc::register_publisher,
                   cc::register_request_timeout);
    driver.add(publisher, true);
}

TEST_F(DriverTest, SendSubscriberRegistration)
{
    expect_request("bradbury_sub",
                   subscriber,
                   cc::register_subscriber,
                   cc::register_request_timeout);
    driver.add(subscriber, false);
}

TEST_F(DriverTest, SendPublisherRemoval)
{
    expect_request("bradbury_pub",
                   publisher,
                   cc::remove_publisher,
                   cc::remove_request_timeout);
    driver.remove(publisher, true);
}

TEST_F(DriverTest, SendSubscriberRemoval)
{
    expect_request("bradbury_sub",
                   subscriber,
                   cc::remove_subscriber,
                   cc::remove_request_timeout);
    driver.remove(subscriber, false);
}

TEST_F(DriverTest, SendHostRemoval)
{
    expect_request("10.2.9.1_node",
                   "10.2.9.1",
                   cc::remove_all_registration,
                   cc::remove_request_timeout);
    driver.remove_all("10.2.9.1_node", "10.2.9.1");
}


TEST_F(DriverTest, SendPublisherFind)
{
    auto data = t::new_registration("10.2.9.1_node", "10.2.9.1",
                                    "bradbury:scifi:books", true);
    expect_request("10.2.9.1_node",
                   data,
                   cc::find_publisher,
                   cc::find_request_timeout);
    driver.find(data, true);
}


TEST_F(DriverTest, SendSubscriberFind)
{
    auto data = t::new_registration("10.2.9.1_node", "10.2.9.1",
                                    "bradbury:scifi:books", false);
    expect_request("10.2.9.1_node",
                   data,
                   cc::find_subscriber,
                   cc::find_request_timeout);
    driver.find(data, false);
}


TEST_F(DriverTest, GetRegistration)
{
    auto all_reg = cm::RegDataSet{publisher, subscriber};
    auto data = t::new_registration("10.2.9.1_node", "10.2.9.1",
                                    "bradbury:scifi:books", false);
    set_response({"", "", all_reg}); // NOLINT

    auto res = driver.find(data, false);

    EXPECT_THAT(res, ContainerEq(all_reg));
}



int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
