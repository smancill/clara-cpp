#include <clara/msg/topic.hpp>

#include "helper/utils.hpp"

#include <gmock/gmock.h>

#include <stdexcept>
#include <string>

using namespace testing;

using clara::msg::Topic;

using TopicStr = std::string;
using TopicArgs = std::vector<std::string>;


auto build(const TopicArgs& args) -> TopicStr
{
    switch (args.size()) {
        case 1: return Topic::build(args[0]).str();
        case 2: return Topic::build(args[0], args[1]).str();
        case 3: return Topic::build(args[0], args[1], args[2]).str();
        default: throw std::logic_error{"invalid number of params"};
    }
}


using TopicBuilderParam = std::pair<TopicArgs, TopicStr>;
PARAMETERIZED_SUITE(TopicBuildSuite, TopicBuilderParam)
{
    return {
        // basic
        {{"rock"}, {"rock"}},
        {{"rock", "metal"}, {"rock:metal"}},
        {{"rock", "metal", "metallica"}, {"rock:metal:metallica"}},
        // extended parts
        {{"rock", "metal", "metallica:lars:*"}, {"rock:metal:metallica:lars"}},
        {{"rock", "metal", "metallica:lars:james"}, {"rock:metal:metallica:lars:james"}},
        // undefined values
        {{"rock", "*"}, {"rock"}},
        {{"rock", "*", "*"}, {"rock"}},
        {{"rock", "metal", "*"}, {"rock:metal"}},
    };
}

TEST_P(TopicBuildSuite, BuildTopicFromParts)
{
    const auto& [args, expected] = GetParam();

    ASSERT_THAT(build(args), Eq(expected));
}


PARAMETERIZED_SUITE(TopicBuildErrorSuite, TopicArgs)
{
    return {
        {"*"},
        {"*", "master"},
        {"rock", "*", "metallica"},
    };
}

TEST_P(TopicBuildErrorSuite, BuildTopicFromPartsThrowsOnInvalidArgs)
{
    const auto& args = GetParam();

    ASSERT_THROW(build(args), std::invalid_argument);
}


using TopicRawParam = std::pair<TopicStr, TopicStr>;
PARAMETERIZED_SUITE(TopicWrapSuite, TopicRawParam)
{
    return {
        {{"rock"}, {"rock"}},
        {{"rock:metal"}, {"rock:metal"}},
        {{"rock:metal:metallica"}, {"rock:metal:metallica"}},
        {{"rock:metal:metallica:lars:james"}, {"rock:metal:metallica:lars:james"}},
    };
}

TEST_P(TopicWrapSuite, BuildTopicFromRawString)
{
    const auto& [arg, expected] = GetParam();

    ASSERT_THAT(Topic::raw(arg).str(), StrEq(expected));
}


using TopicPartsParam = std::pair<TopicStr, TopicArgs>;
PARAMETERIZED_SUITE(TopicPartsSuite, TopicPartsParam)
{
    return {
        {{"rock"}, {"rock", Topic::ANY, Topic::ANY}},
        {{"rock:metal"}, {"rock", "metal", Topic::ANY}},
        {{"rock:metal:metallica"}, {"rock", "metal", "metallica"}},
        {{"rock:metal:metallica:lars"}, {"rock", "metal", "metallica:lars"}},
    };
}

TEST_P(TopicPartsSuite, GetTopicParts)
{
    const auto& [arg, expected] = GetParam();

    auto topic = Topic::raw(arg);

    EXPECT_THAT(topic.domain(), StrEq(expected[0]));
    EXPECT_THAT(topic.subject(), StrEq(expected[1]));
    EXPECT_THAT(topic.type(), StrEq(expected[2]));
}


using TopicParentParam = std::tuple<TopicStr, TopicStr, bool>;
PARAMETERIZED_SUITE(TopicParentSuite, TopicParentParam)
{
    return {
        {{"rock"}, {"rock"}, true},
        {{"rock"}, {"rock:metal"}, true},
        {{"rock"}, {"rock:metal:slayer"}, true},

        {{"rock"}, {"movies"}, false},
        {{"rock"}, {"movies:classic"}, false},
        {{"rock"}, {"movies:classic:casablanca"}, false},

        {{"rock:metal"}, {"rock:metal"}, true},
        {{"rock:metal"}, {"rock:metal:metallica"}, true},

        {{"rock:metal"}, {"rock"}, false},
        {{"rock:metal"}, {"rock:alternative"}, false},
        {{"rock:metal"}, {"rock:alternative:nirvana"}, false},

        {{"rock:metal"}, {"movies"}, false},
        {{"rock:metal"}, {"movies:thriller"}, false},
        {{"rock:metal"}, {"movies:thriller:shark"}, false},

        {{"rock:metal:metallica"}, {"rock:metal:metallica"}, true},

        {{"rock:metal:metallica"}, {"rock"}, false},

        {{"rock:metal:metallica"}, {"rock:metal"}, false},
        {{"rock:metal:metallica"}, {"rock:metal:slayer"}, false},

        {{"rock:metal:metallica"}, {"rock:alternative"}, false},
        {{"rock:metal:metallica"}, {"rock:alternative:nirvana"}, false},

        {{"rock:metal:metallica"}, {"movies"}, false},
        {{"rock:metal:metallica"}, {"movies:thriller"}, false},
        {{"rock:metal:metallica"}, {"movies:thriller:shark"}, false},
    };
}

TEST_P(TopicParentSuite, IsParentTopic)
{
    const auto& [lhs, rhs, expected] = GetParam();
    auto topic = Topic::raw(lhs);
    auto other = Topic::raw(rhs);

    ASSERT_THAT(topic.is_parent(other), Eq(expected));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
