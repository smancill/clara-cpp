#include <clara/engine_data.hpp>
#include <clara/engine_data_type.hpp>

#include <engine_data_helper.hpp>
#include <clara/msg/message.hpp>

#include <gmock/gmock.h>

using namespace testing;


TEST(EngineData, ReadFromMessage)
{
    auto mt = clara::type::STRING.mime_type();
    auto e = clara::EngineDataAccessor{};

    auto m = clara::msg::proto::make_meta();
    m->set_datatype(mt);
    m->set_status(clara::msg::proto::Meta::INFO);
    m->set_severityid(2);
    m->set_author("host/cont/service");
    m->set_version("2.0");
    m->set_communicationid(2000);
    m->set_composition("composition string");
    m->set_executiontime(24500);

    auto o = std::any{std::string{"some text"}};

    auto d = e.create(std::move(o), std::move(m));

    EXPECT_THAT(d.mime_type(), Eq(mt));
    EXPECT_THAT(d.status(), Eq(clara::EngineStatus::INFO));
    EXPECT_THAT(d.status_severity(), Eq(2));

    EXPECT_THAT(d.engine_name(), StrEq("host/cont/service"));
    EXPECT_THAT(d.engine_version(), StrEq("2.0"));

    EXPECT_THAT(d.communication_id(), Eq(2000));
    EXPECT_THAT(d.composition(), StrEq("composition string"));
    EXPECT_THAT(d.execution_time(), Eq(24500));
}


TEST(EngineData, SetMimeTypeFromString)
{
    auto d = clara::EngineData{};
    auto m = clara::type::SFIXED32.mime_type();

    d.set_data(m, 600);

    EXPECT_THAT(d.mime_type(), Eq(m));
}


TEST(EngineData, SetMimeTypeFromDataType)
{
    auto d = clara::EngineData{};
    auto t = clara::type::SFIXED32;

    d.set_data(t, 600);

    EXPECT_THAT(d.mime_type(), Eq(t.mime_type()));
}


TEST(EngineData, CreateFromPrimitive)
{
    auto d = clara::EngineData{};

    d.set_data(clara::type::SFIXED32, 800);

    EXPECT_THAT(clara::data_cast<std::int32_t>(d), Eq(800));
}


TEST(EngineData, CreateFromString)
{
    auto d = clara::EngineData{};
    auto s = std::string{"Ash nazg durbatulûk"};

    d.set_data(clara::type::STRING, s);

    EXPECT_THAT(clara::data_cast<std::string>(d), StrEq("Ash nazg durbatulûk"));
}


TEST(EngineData, CreateFromStringLiteral)
{
    auto d = clara::EngineData{};

    d.set_data(clara::type::STRING, "Ash nazg durbatulûk");

    EXPECT_THAT(clara::data_cast<std::string>(d), StrEq("Ash nazg durbatulûk"));
}


TEST(EngineData, CreateFromVector)
{
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};

    d.set_data(clara::type::ARRAY_SFIXED32, v);

    EXPECT_THAT(clara::data_cast<std::vector<std::int32_t>>(d), Eq(v));
}


TEST(EngineData, CreateFromMovedData)
{
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};
    auto t = v;

    d.set_data(clara::type::ARRAY_SFIXED32, std::move(t));

    EXPECT_THAT(t, IsEmpty());
    EXPECT_THAT(clara::data_cast<std::vector<std::int32_t>>(d), Eq(v));
}


TEST(EngineData, CopyConstruction)
{
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};

    d.set_data(clara::type::ARRAY_SFIXED32, v);
    d.set_description("numbers");

    auto c = d;

    EXPECT_THAT(clara::data_cast<std::vector<std::int32_t>>(d), Eq(v));
    EXPECT_THAT(d.description(), StrEq("numbers"));

    EXPECT_THAT(clara::data_cast<std::vector<std::int32_t>>(c), Eq(v));
    EXPECT_THAT(c.description(), StrEq("numbers"));
}


TEST(EngineData, MoveConstruction)
{
    auto e = clara::EngineDataAccessor{};
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};

    d.set_data(clara::type::ARRAY_SFIXED32, v);
    d.set_description("numbers");

    auto c = std::move(d);

    EXPECT_THAT(clara::data_cast<decltype(v)>(c), Eq(v));
    EXPECT_THAT(c.description(), StrEq("numbers"));

    EXPECT_THAT(e.view_meta(d), Eq(nullptr));
}


TEST(EngineData, HasData)
{
    auto d = clara::EngineData{};
    EXPECT_THAT(d.has_data(), Eq(false));

    d.set_data("data/integer", 10);
    EXPECT_THAT(d.has_data(), Eq(true));
}


TEST(EngineData, GetDataByReference)
{
    struct A {
        int value = 0;
    };

    auto d = clara::EngineData{};
    d.set_data("test/A", A{});

    auto& a = clara::data_cast<A>(d);
    a.value = 1;

    auto& b = clara::data_cast<A>(d);

    EXPECT_THAT(b.value, Eq(1));
}


TEST(EngineData, SetDataDescription)
{
    auto d = clara::EngineData{};

    d.set_description("data description");

    EXPECT_THAT(d.description(), StrEq("data description"));
}


TEST(EngineData, SetRequestStatus)
{
    auto d = clara::EngineData{};

    d.set_status(clara::EngineStatus::INFO);

    EXPECT_THAT(d.status(), Eq(clara::EngineStatus::INFO));
    EXPECT_THAT(d.status_severity(), Eq(1));

    d.set_status(clara::EngineStatus::ERROR, 2);

    EXPECT_THAT(d.status(), Eq(clara::EngineStatus::ERROR));
    EXPECT_THAT(d.status_severity(), Eq(2));
}


TEST(EngineData, SetEngineState)
{
    auto d = clara::EngineData{};

    d.set_engine_state("all clear");

    EXPECT_THAT(d.engine_state(), StrEq("all clear"));
}


TEST(EngineData, SetCommunicationId)
{
    auto d = clara::EngineData{};

    d.set_communication_id(34000);

    EXPECT_THAT(d.communication_id(), Eq(34000));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
