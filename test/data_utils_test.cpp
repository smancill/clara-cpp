#include "data_utils.hpp"

#include <gmock/gmock.h>

namespace cm = clara::msg;
namespace util = clara::util;

using namespace std::string_literals;
using namespace testing;


auto request(std::string_view data) -> cm::Message
{
    auto topic = cm::Topic::raw("topic");
    auto bytes = std::vector<std::uint8_t>{data.begin(), data.end()};
    return cm::Message{std::move(topic), "text/string", std::move(bytes)};
}


TEST(RequestParser, ParseContainerRequest)
{
    auto r = "startContainer?master?5?undefined"s;
    auto m = request(r);
    auto p = util::RequestParser::build(m);

    EXPECT_THAT(p.next_string(),  StrEq("startContainer"));
    EXPECT_THAT(p.next_string(),  StrEq("master"));
    EXPECT_THAT(p.next_integer(), Eq(5));
    EXPECT_THAT(p.next_string(),  StrEq("undefined"));

    EXPECT_THAT(p.request(), Eq(r));
}


TEST(RequestParser, ParseServiceRequest)
{
    auto r = "startService?master?E1?org.jlab.clara.examples.engines.E1?3?undefined?undefined"s;
    auto m = request(r);
    auto p = util::RequestParser::build(m);

    EXPECT_THAT(p.next_string(),  StrEq("startService"));
    EXPECT_THAT(p.next_string(),  StrEq("master"));
    EXPECT_THAT(p.next_string(),  StrEq("E1"));
    EXPECT_THAT(p.next_string(),  StrEq("org.jlab.clara.examples.engines.E1"));
    EXPECT_THAT(p.next_integer(), Eq(3));
    EXPECT_THAT(p.next_string(),  StrEq("undefined"));
    EXPECT_THAT(p.next_string(),  StrEq("undefined"));

    EXPECT_THAT(p.request(), Eq(r));
}


TEST(RequestParser, InvalidMimeType)
{
    auto m = cm::make_message(cm::Topic::raw("topic"), 34.8);

    ASSERT_THROW(util::RequestParser::build(m), util::InvalidRequest);
}


TEST(RequestParser, EmptyRequest)
{
    auto m = request("");
    auto p = util::RequestParser::build(m);

    ASSERT_THROW(p.next_string(), util::InvalidRequest);
}


TEST(RequestParser, SingleStringElement)
{
    auto m = request("pingDpe");
    auto p = util::RequestParser::build(m);

    EXPECT_THAT(p.next_string(), StrEq("pingDpe"));
}


TEST(RequestParser, MissingStringElement)
{
    auto m = request("stopService?master?E1");
    auto p = util::RequestParser::build(m);

    p.next_string();
    p.next_string();
    p.next_string();

    ASSERT_THROW(p.next_string(), util::InvalidRequest);
}


TEST(RequestParser, EmptyStringElement)
{
    auto m = request("stopService??E1");
    auto p = util::RequestParser::build(m);

    p.next_string();

    ASSERT_THROW(p.next_string(), util::InvalidRequest);
}


TEST(RequestParser, DefaultMissingStringElement)
{
    auto m = request("stopService?master");
    auto p = util::RequestParser::build(m);

    p.next_string();
    p.next_string();

    EXPECT_THAT(p.next_string("E1"), StrEq("E1"));
}


TEST(RequestParser, DefaultEmptyStringElement)
{
    auto m = request("stopService??E1");
    auto p = util::RequestParser::build(m);

    p.next_string();

    EXPECT_THAT(p.next_string("master"), StrEq("master"));
    EXPECT_THAT(p.next_string(), StrEq("E1"));
}


TEST(RequestParser, MissingIntegerElement)
{
    auto m = request("stopService?master?E1");
    auto p = util::RequestParser::build(m);

    p.next_string();
    p.next_string();
    p.next_string();

    ASSERT_THROW(p.next_integer(), util::InvalidRequest);
}


TEST(RequestParser, EmptyIntegerElement)
{
    auto m = request("stopService??E1");
    auto p = util::RequestParser::build(m);

    p.next_string();

    ASSERT_THROW(p.next_integer(), util::InvalidRequest);
}


TEST(RequestParser, NotAnInteger)
{
    auto m = request("stopService?master?E1");
    auto p = util::RequestParser::build(m);

    ASSERT_THROW(p.next_integer(), util::InvalidRequest);
}


TEST(RequestParser, OnlySeparators)
{
    auto m = request("???");
    auto p = util::RequestParser::build(m);

    ASSERT_THROW(p.next_string(), util::InvalidRequest);
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
