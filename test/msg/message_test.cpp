/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/msg/message.hpp>

#include "helper/utils.hpp"

#include <gmock/gmock.h>

using namespace testing;

namespace cm = clara::msg;
namespace mt = clara::msg::mimetype;

namespace {

auto make_test_data()
{
    auto data = cm::proto::Data{};
    data.set_flsint32(29);
    data.set_float_(42.F);
    data.set_string("november");
    data.add_doublea(0.);
    data.add_doublea(9.);
    return data;
}

const auto topic = cm::Topic::raw("test/topic");

}


TEST(Data, Serialize)
{
    auto data = make_test_data();
    auto buffer = cm::proto::serialize_data(data);
    auto result = cm::proto::parse_data(buffer);

    EXPECT_THAT(result, Eq(data));
}


TEST(Message, CreateWithBytes)
{
    auto data = std::vector<std::uint8_t>{0x0, 0x1, 0x2, 0x3, 0xa, 0xb};
    auto msg = cm::Message{topic, "test/binary", data};

    EXPECT_THAT(msg.data(), ContainerEq(data));
    EXPECT_THAT(msg.meta()->datatype(), StrEq("test/binary"));
}


TEST(Message, PassingNullMetadataThrows)
{
    auto data = std::vector<std::uint8_t>{0x0, 0x1, 0x2, 0x3, 0xa, 0xb};
    auto meta = std::unique_ptr<cm::proto::Meta>{};

    EXPECT_EXCEPTION(cm::Message(topic, std::move(meta), data),
                     std::invalid_argument, "null metadata");
}


TEST(Message, EqualMessages)
{
    auto data = std::vector<std::uint8_t>{0x0, 0x1, 0x2, 0x3, 0xa, 0xb};

    auto msg1 = cm::Message{topic, "test/binary", data};
    auto msg2 = cm::Message{topic, "test/binary", data};

    ASSERT_THAT(msg1, Eq(msg2));
}


TEST(Message, CreateCopy)
{
    auto data = std::vector<std::uint8_t>{0x0, 0x1, 0x2, 0x3, 0xa, 0xb};

    auto msg1 = cm::Message{topic, "test/binary", data};
    auto msg2 = cm::Message{msg1};

    ASSERT_THAT(msg1, Eq(msg2));
}


TEST(Message, SwapMessages)
{
    auto topic1 = cm::Topic::raw("topic1");
    auto topic2 = cm::Topic::raw("topic2");

    auto data1 = std::vector<std::uint8_t>{0x0, 0x1, 0x2, 0x3, 0xa, 0xb};
    auto data2 = std::vector<std::uint8_t>{0x0, 0x4, 0x5, 0x6, 0xc, 0xd};

    auto msg1 = cm::Message{topic1, "test/binary1", data1};
    auto msg2 = cm::Message{topic2, "test/binary2", data2};

    auto copy1 = msg1;
    auto copy2 = msg2;

    swap(copy1, copy2);

    ASSERT_THAT(copy1, Eq(msg2));
    ASSERT_THAT(copy2, Eq(msg1));
}


TEST(Message, CreateWithProtoData)
{
    auto data = make_test_data();
    auto msg = cm::make_message(topic, data);
    auto result = cm::parse_message<cm::proto::Data>(msg);

    EXPECT_THAT(result, Eq(data));
    EXPECT_THAT(msg.meta()->datatype(), StrEq(mt::plain_data));
}


TEST(Message, CreateWithIntegerData)
{
    auto msg = cm::make_message(topic, 42);
    auto result = cm::parse_message<std::int32_t>(msg);

    EXPECT_THAT(result, Eq(42));
    EXPECT_THAT(msg.meta()->datatype(), StrEq(mt::single_sfixed32));
}


TEST(Message, CreateWithFloatData)
{
    auto data = 4.8F;
    auto msg = cm::make_message(topic, data);
    auto result = cm::parse_message<float>(msg);

    EXPECT_THAT(result, Eq(data));
    EXPECT_THAT(msg.meta()->datatype(), StrEq(mt::single_float));
}


TEST(Message, CreateWithStringData)
{
    auto data = "some_string";
    auto msg = cm::make_message(topic, data);
    auto result = cm::parse_message<std::string>(msg);

    EXPECT_THAT(result, Eq(data));
    EXPECT_THAT(msg.meta()->datatype(), StrEq(mt::single_string));
}


TEST(Message, CreateWithIntegerArray)
{
    auto data = std::vector<std::int64_t>{1000L, 2000L, 3000L};
    auto msg = cm::make_message(topic, data);
    auto result = cm::parse_message<std::vector<std::int64_t>>(msg);

    EXPECT_THAT(result, ContainerEq(data));
    EXPECT_THAT(msg.meta()->datatype(), StrEq(mt::array_sfixed64));
}


TEST(Message, CreateWithFloatArray)
{
    auto data = std::vector<double>{1000., 2000., 3000.};
    auto msg = cm::make_message(topic, data);
    auto result = cm::parse_message<std::vector<double>>(msg);

    EXPECT_THAT(result, ContainerEq(data));
    EXPECT_THAT(msg.meta()->datatype(), StrEq(mt::array_double));
}


TEST(Message, CreateWithStringArray)
{
    auto data = std::vector<std::string>{"one", "two", "three"};
    auto msg = cm::make_message(topic, data);
    auto result = cm::parse_message<std::vector<std::string>>(msg);

    EXPECT_THAT(result, ContainerEq(data));
    EXPECT_THAT(msg.meta()->datatype(), StrEq(mt::array_string));
}


TEST(Message, CreateCopyResponse)
{
    auto data = std::vector<std::uint8_t>{0x0, 0x1, 0x2, 0x3, 0xa, 0xb};
    auto meta = cm::proto::make_meta();
    meta->set_replyto("return_123");
    meta->set_datatype("test/binary");
    auto msg = cm::Message{topic, std::move(meta), data};

    auto res_msg = cm::make_response(msg);

    ASSERT_THAT(res_msg.topic().str(), StrEq("return_123"));
    ASSERT_THAT(res_msg.data(), ContainerEq(data));
    ASSERT_THAT(res_msg.datatype(), StrEq("test/binary"));
}


TEST(Message, CreateMoveResponse)
{
    auto data = std::vector<std::uint8_t>{0x0, 0x1, 0x2, 0x3, 0xa, 0xb};
    auto meta = cm::proto::make_meta();
    meta->set_replyto("return_123");
    meta->set_datatype("test/binary");
    auto msg = cm::Message{topic, std::move(meta), data};

    auto res_msg = cm::make_response(std::move(msg));

    ASSERT_THAT(res_msg.topic().str(), StrEq("return_123"));
    ASSERT_THAT(res_msg.data(), ContainerEq(data));
    ASSERT_THAT(res_msg.datatype(), StrEq("test/binary"));

    ASSERT_THAT(msg.topic().str(), StrEq(""));  // NOLINT(bugprone-use-after-move)
    ASSERT_THAT(msg.meta(), Eq(nullptr));
    ASSERT_THAT(msg.data(), IsEmpty());
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
