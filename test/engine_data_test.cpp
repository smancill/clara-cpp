/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set ts=8 sts=4 et sw=4 tw=80: */

#include <clara/engine_data.hpp>
#include <clara/engine_data_type.hpp>

#include <engine_data_helper.hpp>
#include <xmsg/message.h>

#include <gmock/gmock.h>

using namespace testing;


TEST(EngineData, ReadFromMessage) {
    auto mt = clara::type::STRING.mime_type();
    auto e = clara::EngineDataAccessor{};

    auto m = xmsg::proto::make_meta();
    m->set_datatype(mt);
    m->set_status(xmsg::proto::Meta::INFO);
    m->set_severityid(2);
    m->set_author("host/cont/service");
    m->set_version("2.0");
    m->set_communicationid(2000);
    m->set_composition("composition string");
    m->set_executiontime(24500);

    auto o = clara::any{std::string{"some text"}};

    auto d = e.create(std::move(o), std::move(m));

    EXPECT_THAT(d.mime_type(), Eq(mt));
    EXPECT_THAT(d.status(), Eq(clara::EngineStatus::INFO));
    EXPECT_THAT(d.status_severity(), Eq(2));

    EXPECT_THAT(d.engine_name(), Eq("host/cont/service"));
    EXPECT_THAT(d.engine_version(), Eq("2.0"));

    EXPECT_THAT(d.communication_id(), Eq(2000));
    EXPECT_THAT(d.composition(), Eq("composition string"));
    EXPECT_THAT(d.execution_time(), Eq(24500));
}


TEST(EngineData, CreateFromPrimitive)
{
    auto d = clara::EngineData{};

    d.set_data(clara::type::SFIXED32.mime_type(), 800);

    EXPECT_THAT(d.data<std::int32_t>(), Eq(800));
}


TEST(EngineData, CreateFromString)
{
    auto d = clara::EngineData{};

    d.set_data(clara::type::STRING.mime_type(), "Ash nazg durbatulûk");

    EXPECT_THAT(d.data<std::string>(), Eq("Ash nazg durbatulûk"));
}


TEST(EngineData, CreateFromVector)
{
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};

    d.set_data(clara::type::ARRAY_SFIXED32.mime_type(), v);

    EXPECT_THAT(d.data<std::vector<std::int32_t>>(), Eq(v));
}


TEST(EngineData, CreateFromMovedData)
{
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};
    auto t = v;

    d.set_data(clara::type::ARRAY_SFIXED32.mime_type(), std::move(t));

    EXPECT_THAT(t, IsEmpty());
    EXPECT_THAT(d.data<std::vector<std::int32_t>>(), Eq(v));
}


TEST(EngineData, CopyConstruction)
{
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};

    d.set_data(clara::type::ARRAY_SFIXED32.mime_type(), v);
    d.set_description("numbers");

    auto c = d;

    EXPECT_THAT(d.data<std::vector<std::int32_t>>(), Eq(v));
    EXPECT_THAT(d.description(), Eq("numbers"));

    EXPECT_THAT(c.data<std::vector<std::int32_t>>(), Eq(v));
    EXPECT_THAT(c.description(), Eq("numbers"));
}


TEST(EngineData, MoveConstruction)
{
    auto e = clara::EngineDataAccessor{};
    auto d = clara::EngineData{};
    auto v = std::vector<std::int32_t>{1, 2, 3, 4, 5, 6};

    d.set_data(clara::type::ARRAY_SFIXED32.mime_type(), v);
    d.set_description("numbers");

    auto c = std::move(d);

    EXPECT_THAT(c.data<std::vector<std::int32_t>>(), Eq(v));
    EXPECT_THAT(c.description(), Eq("numbers"));

    EXPECT_THAT(e.view_meta(d), Eq(nullptr));
}


TEST(EngineData, SetDataDescription)
{
    auto d = clara::EngineData{};

    d.set_description("data description");

    EXPECT_THAT(d.description(), Eq("data description"));
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

    EXPECT_THAT(d.engine_state(), Eq("all clear"));
}


TEST(EngineData, SetCommunicationId)
{
    auto d = clara::EngineData{};

    d.set_communication_id(34000);

    EXPECT_THAT(d.communication_id(), Eq(34000));
}


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
