/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <clara/engine_data_type.hpp>

#include <clara/msg/proto/data.hpp>

#include <gmock/gmock.h>

#include <algorithm>
#include <climits>
#include <functional>
#include <random>
#include <string>
#include <vector>

using namespace testing;


TEST(PrimitiveSerializer, IntegerSerialization)
{
    const auto* s = clara::type::INT32.serializer();

    const auto b = s->write(std::any{18});
    const auto d = std::any_cast<std::int32_t>(s->read(b));

    ASSERT_THAT(d, Eq(18));
}


TEST(PrimitiveSerializer, FloatingPointSerialization)
{
    const auto* s = clara::type::DOUBLE.serializer();

    const auto b = s->write(std::any{78.98});
    const auto d = std::any_cast<double>(s->read(b));

    ASSERT_THAT(d, Eq(78.98));
}


TEST(PrimitiveSerializer, StringSerialization)
{
    const auto* s = clara::type::STRING.serializer();

    const auto b = s->write(std::any{std::string{"Master of Puppets"}});
    const auto d = std::any_cast<std::string>(s->read(b));

    ASSERT_THAT(d, StrEq("Master of Puppets"));
}


TEST(RawBytesSerializer, RawBytesSerialization)
{
    const auto* s = clara::type::BYTES.serializer();

    using rnd_eng = std::default_random_engine;
    using rnd_byte_eng = std::independent_bits_engine<rnd_eng, CHAR_BIT, std::uint8_t>;
    auto rbe = rnd_byte_eng{};

    auto r = std::vector<std::uint8_t>(100);
    std::generate(r.begin(), r.end(), std::ref(rbe));

    const auto b = s->write(std::any{r});
    const auto d = std::any_cast<decltype(r)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(r));
}


TEST(RawBytesSerializer, MoveSemantics)
{
    const auto* s = clara::type::BYTES.serializer();

    using rnd_eng = std::default_random_engine;
    using rnd_byte_eng = std::independent_bits_engine<rnd_eng, CHAR_BIT, std::uint8_t>;
    auto rbe = rnd_byte_eng{};

    auto r = std::vector<std::uint8_t>(100);
    std::generate(r.begin(), r.end(), std::ref(rbe));

    auto a = std::any{r};

    auto* m = std::any_cast<decltype(r)>(&a);
    ASSERT_THAT(*m, ContainerEq(r));

    auto b = s->write(std::move(a));
    auto d = std::any_cast<decltype(r)>(s->read(std::move(b)));

    ASSERT_THAT(*m, IsEmpty());
    ASSERT_THAT(b, IsEmpty());
    ASSERT_THAT(d, ContainerEq(r));
}


TEST(JSONSerializer, JSONSerialization)
{
    const auto* s = clara::type::JSON.serializer();

    auto j = std::string{"{ \"a\": 1, \"b\": 2, \"c\": [ 3, 4, 5] }"};

    const auto b = s->write(std::any{j});
    const auto d = std::any_cast<decltype(j)>(s->read(b));

    ASSERT_THAT(d, Eq(j));
}


TEST(EngineDataType, CompareWithStringMimeType)
{
    ASSERT_THAT(clara::type::JSON, Eq("application/json"));
    ASSERT_THAT(clara::type::JSON, Ne("application/xml"));

    ASSERT_THAT("binary/bytes", Eq(clara::type::BYTES));
    ASSERT_THAT("binary/bytes", Ne(clara::type::STRING));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
