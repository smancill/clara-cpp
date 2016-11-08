/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set ts=8 sts=4 et sw=4 tw=80: */

#include "utils.hpp"

#include <gmock/gmock.h>

using namespace clara::util;
using namespace testing;

const std::string name = "192.168.1.1_java:master:engine";
const std::string name_full = "192.168.1.2%9999_cpp:master:engine";


TEST(ClaraNames, GetDpeName)
{
    ASSERT_THAT(get_dpe_name(name), Eq("192.168.1.1_java"));
}


TEST(ClaraNames, GetContainerName)
{
    ASSERT_THAT(get_container_name(name), Eq("master"));
}


TEST(ClaraNames, GetContainerCanonicalName)
{
    ASSERT_THAT(get_container_canonical_name(name),
                Eq("192.168.1.1_java:master"));
}


TEST(ClaraNames, GetEngineName)
{
    ASSERT_THAT(get_engine_name(name), Eq("engine"));
}


TEST(ClaraNames, GetDpeHost)
{
    ASSERT_THAT(get_dpe_host(name), Eq("192.168.1.1"));
    ASSERT_THAT(get_dpe_host(name_full), Eq("192.168.1.2"));
}


TEST(ClaraNames, GetDpePort)
{
    ASSERT_THAT(get_dpe_port(name), Eq(7771));
    ASSERT_THAT(get_dpe_port(name_full), Eq(9999));
}


TEST(ClaraNames, GetDpeLang)
{
    ASSERT_THAT(get_dpe_lang(name), Eq("java"));
    ASSERT_THAT(get_dpe_lang(name_full), Eq("cpp"));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
