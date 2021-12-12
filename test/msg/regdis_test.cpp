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


template<typename R>
auto move_msg(R& r) -> R
{
    return R{r.msg()};
}


TEST(Request, CreateDataRequest)
{
    auto data = t::new_registration("asimov", "10.2.9.1", "writer:scifi", true);

    auto send_req = Request{"foo:bar", "foo_service", data};
    auto recv_req = move_msg(send_req);

    EXPECT_THAT(recv_req.topic(), StrEq("foo:bar"));
    EXPECT_THAT(recv_req.sender(), StrEq("foo_service"));
    EXPECT_THAT(recv_req.data(), Eq(data));
}


TEST(Request, CreateTextRequest)
{
    auto send_req = Request{"foo:bar", "foo_service", "10.2.9.2"};
    auto recv_req = move_msg(send_req);

    EXPECT_THAT(recv_req.topic(), StrEq("foo:bar"));
    EXPECT_THAT(recv_req.sender(), StrEq("foo_service"));
    EXPECT_THAT(recv_req.text(), StrEq("10.2.9.2"));
}


TEST(Response, CreateSuccessResponse)
{
    auto send_res = Response{"foo:bar", "registration_fe"};
    auto recv_res = move_msg(send_res);

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
    auto recv_res = move_msg(send_res);

    EXPECT_THAT(recv_res.topic(), StrEq("foo:bar"));
    EXPECT_THAT(recv_res.sender(), StrEq("registration_fe"));
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
      , subscriber{t::new_registration("bradbury_sub", "localhost",
                                       "writer:scifi:books", false)}
      , publisher{t::new_registration("bradbury_pub", "localhost",
                                      "writer:scifi:books", true)}
    {
        ON_CALL(driver, request(_, _))
            .WillByDefault(Return(Response{"", "", ""}));
    }

    void set_response(const cm::RegDataSet& data)
    {
        EXPECT_CALL(driver, request(_, _)).WillOnce(Return(Response{"", "", data}));
    }

    static auto make_request(const std::string& topic,
                             const cm::proto::Registration& data) -> Request
    {
        return {topic, data.name(), data};
    }

    void expect_request(const Request& reg, int timeout)
    {
        EXPECT_CALL(driver, request(Eq(reg), Eq(timeout)));
    }

    MockContext ctx;
    MockDriver driver;

    cm::proto::Registration subscriber;
    cm::proto::Registration publisher;
};


TEST_F(DriverTest, SendPublisherRegistration)
{
    auto req = make_request(cc::register_publisher, publisher);

    expect_request(req, cc::register_request_timeout);

    driver.add(publisher, true);
}


TEST_F(DriverTest, SendSubscriberRegistration)
{
    auto req = make_request(cc::register_subscriber, subscriber);

    expect_request(req, cc::register_request_timeout);

    driver.add(subscriber, false);
}


TEST_F(DriverTest, SendPublisherRemoval)
{
    auto req = make_request(cc::remove_publisher, publisher);

    expect_request(req, cc::remove_request_timeout);

    driver.remove(publisher, true);
}


TEST_F(DriverTest, SendSubscriberRemoval)
{
    auto req = make_request(cc::remove_subscriber, subscriber);

    expect_request(req, cc::remove_request_timeout);

    driver.remove(subscriber, false);
}


TEST_F(DriverTest, SendHostRemoval)
{
    auto req = Request{cc::remove_all_registration, "10.2.9.1_node", "10.2.9.1"};

    expect_request(req, cc::remove_request_timeout);

    driver.remove_all("10.2.9.1_node", "10.2.9.1");
}


TEST_F(DriverTest, SendPublisherFind)
{
    auto data = t::new_registration(
            "10.2.9.1_node", "10.2.9.1", "bradbury:scifi:books", true);
    auto req = make_request(cc::find_publisher, data);

    expect_request(req, cc::find_request_timeout);

    driver.find(data, true);
}


TEST_F(DriverTest, SendSubscriberFind)
{
    auto data = t::new_registration(
            "10.2.9.1_node", "10.2.9.1", "bradbury:scifi:books", false);
    auto req = make_request(cc::find_subscriber, data);

    expect_request(req, cc::find_request_timeout);

    driver.find(data, false);
}


TEST_F(DriverTest, GetRegistration)
{
    auto data = t::new_registration(
            "10.2.9.1_node", "10.2.9.1", "bradbury:scifi:books", false);
    auto all_reg = cm::RegDataSet{publisher, subscriber};

    set_response(all_reg);

    auto res = driver.find(data, false);

    EXPECT_THAT(res, ContainerEq(all_reg));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
