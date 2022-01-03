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
    const auto* s = clara::type::SINT32.serializer();

    const auto b = s->write(clara::any{18});
    const auto d = clara::any_cast<std::int32_t>(s->read(b));

    ASSERT_THAT(d, Eq(18));
}


TEST(PrimitiveSerializer, FloatingPointSerialization)
{
    const auto* s = clara::type::DOUBLE.serializer();

    const auto b = s->write(clara::any{78.98});
    const auto d = clara::any_cast<double>(s->read(b));

    ASSERT_THAT(d, Eq(78.98));
}


TEST(PrimitiveSerializer, StringSerialization)
{
    const auto* s = clara::type::STRING.serializer();

    const auto b = s->write(clara::any{std::string{"Master of Puppets"}});
    const auto d = clara::any_cast<std::string>(s->read(b));

    ASSERT_THAT(d, StrEq("Master of Puppets"));
}


TEST(ArraySerializer, IntegerSerialization)
{
    const auto* s = clara::type::ARRAY_SFIXED32.serializer();

    const auto v = std::vector<std::int32_t>{4, 5, 6};
    const auto b = s->write(clara::any{v});
    const auto d = clara::any_cast<decltype(v)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(v));
}


TEST(ArraySerializer, FloatingPointSerialization)
{
    const auto* s = clara::type::ARRAY_DOUBLE.serializer();

    const auto v = std::vector<double>{4.1, 5.6};
    const auto b = s->write(clara::any{v});
    const auto d = clara::any_cast<decltype(v)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(v));
}


TEST(ArraySerializer, StringSerialization)
{
    const auto* s = clara::type::ARRAY_STRING.serializer();

    const auto v = std::vector<std::string>{ "Ride the Lightning",
                                             "Master of Puppets",
                                             "...And Justice for All"};
    const auto b = s->write(clara::any{v});
    const auto d = clara::any_cast<decltype(v)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(v));
}


TEST(RawBytesSerializer, RawBytesSerialization)
{
    const auto* s = clara::type::BYTES.serializer();

    using rnd_eng = std::default_random_engine;
    using rnd_byte_eng = std::independent_bits_engine<rnd_eng, CHAR_BIT, std::uint8_t>;
    auto rbe = rnd_byte_eng{};

    auto r = std::vector<std::uint8_t>(100);
    std::generate(r.begin(), r.end(), std::ref(rbe));

    const auto b = s->write(clara::any{r});
    const auto d = clara::any_cast<decltype(r)>(s->read(b));

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

    auto a = clara::any{r};

    auto* m = clara::any_cast<decltype(r)>(&a);
    ASSERT_THAT(*m, ContainerEq(r));

    auto b = s->write(std::move(a));
    auto d = clara::any_cast<decltype(r)>(s->read(std::move(b)));

    ASSERT_THAT(*m, IsEmpty());
    ASSERT_THAT(b, IsEmpty());
    ASSERT_THAT(d, ContainerEq(r));
}


TEST(NativeSerializer, NativeSerialization)
{
    const auto* s = clara::type::NATIVE.serializer();

    auto xd = clara::msg::proto::Data{};
    xd.set_flsint32(56);
    xd.set_double_(5.6);
    xd.add_stringa("Ride the Lightning");
    xd.add_stringa("Master of Puppets");
    xd.add_stringa("...And Justice for All");

    const auto b = s->write(clara::any{xd});
    const auto d = clara::any_cast<decltype(xd)>(s->read(b));

    ASSERT_THAT(d, Eq(xd));
}


TEST(JSONSerializer, JSONSerialization)
{
    const auto* s = clara::type::JSON.serializer();

    auto j = std::string{"{ \"a\": 1, \"b\": 2, \"c\": [ 3, 4, 5] }"};

    const auto b = s->write(clara::any{j});
    const auto d = clara::any_cast<decltype(j)>(s->read(b));

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
