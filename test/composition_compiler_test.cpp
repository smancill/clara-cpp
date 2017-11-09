/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set ts=8 sts=4 et sw=4 tw=80: */

#include "composition.hpp"
#include <iostream>

#include <gmock/gmock.h>

using namespace clara::composition;
using namespace testing;

using output_set = std::set<std::string>;

const std::string composition = R"(10.10.10.1_java:C:S1+)"
                                R"(10.10.10.1_java:C:S2+)"
                                R"(10.10.10.1_java:C:S3+)"
                                R"(10.10.10.1_java:C:S4;)";

TEST(SimpleCompiler, ServiceAtTheBeginning)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S1"};

    cc.compile(composition);

    auto expected = output_set{"10.10.10.1_java:C:S2"};
    ASSERT_THAT(cc.outputs(), ContainerEq(expected));
}


TEST(SimpleCompiler, ServiceAtTheMiddle)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S2"};

    cc.compile(composition);

    auto expected = output_set{"10.10.10.1_java:C:S3"};
    ASSERT_THAT(cc.outputs(), ContainerEq(expected));
}


TEST(SimpleCompiler, ServiceAtTheEnd)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S4"};

    cc.compile(composition);

    ASSERT_THAT(cc.outputs(), IsEmpty());
}


TEST(SimpleCompiler, MultipleCalls)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S3"};

    cc.compile(composition);

    std::string composition2 = R"(10.10.10.1_java:C:S1+)"
                               R"(10.10.10.1_java:C:S3+)"
                               R"(10.10.10.1_java:C:S5;)";
    cc.compile(composition2);

    auto expected = output_set{"10.10.10.1_java:C:S5"};
    ASSERT_THAT(cc.outputs(), ContainerEq(expected));
}

TEST(CompositionCompiler, ServiceAtTheBeginning)
{
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S1"};
    cc.compile(composition);

    auto expected = output_set{"10.10.10.1_java:C:S2"};
    ASSERT_THAT(cc.get_unconditional_links(), ContainerEq(expected));
}

TEST(CompositionCompiler, ServiceAtTheMiddle)
{
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S2"};
    cc.compile(composition);

    auto expected = output_set{"10.10.10.1_java:C:S3"};
    ASSERT_THAT(cc.get_unconditional_links(), ContainerEq(expected));
}

TEST(CompositionCompiler, ServiceAtTheEnd)
{
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S4"};
    cc.compile(composition);

    ASSERT_THAT(cc.get_unconditional_links().empty(), Eq(true));
}

TEST(CompositionCompiler, LastServiceOnALoop)
{
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S3"};
    std::string composition2 = R"(10.10.10.1_java:C:S1+)"
                                R"(10.10.10.1_java:C:S3+)"
                                R"(10.10.10.1_java:C:S1;)";
    cc.compile(composition2);

    auto expected = output_set{"10.10.10.1_java:C:S1"};
    ASSERT_THAT(cc.get_unconditional_links(), ContainerEq(expected));
}

TEST(CompositionCompiler, SimpleConditionTest) {
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S1"};
    std::string simple =    R"(if(10.10.10.1_java:C:S1=="FOO"){)"
                            R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S2;)"
                            R"(})";

    cc.compile(simple);
    auto owner = ServiceState("10.10.10.1_java:C:S1", "BOO");
    auto input = ServiceState("WHATEVER", "DON'T CARE");

    auto expected = output_set{};

    ASSERT_THAT(cc.get_links(owner, input), ContainerEq(expected));
}

TEST(CompositionCompiler, IfElseConditionTest) {
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S1"};
    std::string cond =  R"(if(10.10.10.1_java:C:S1=="FOO"){)"
                        R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S4;)"
                        R"(}else{)"
                        R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S5;)"
                        R"(})";

    cc.compile(cond);
    auto owner = ServiceState("10.10.10.1_java:C:S1", "BOO");
    auto input = ServiceState("WHATEVER", "DON'T CARE");

    auto expected = output_set{"10.10.10.1_java:C:S5"};

    ASSERT_THAT(cc.get_links(owner, input), ContainerEq(expected));
}

TEST(CompositionCompiler, IfElseIfConditionTest) {
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S1"};
    std::string cond =  R"(if(10.10.10.1_java:C:S1=="FOO"){)"
                        R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S2;)"
                        R"(}elseif(10.10.10.1_java:C:S1=="BOO"){)"
                        R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S3;)"
                        R"(})";

    cc.compile(cond);
    auto owner = ServiceState("10.10.10.1_java:C:S1", "BOO");
    auto input = ServiceState("WHATEVER", "DON'T CARE");

    auto expected = output_set{"10.10.10.1_java:C:S3"};

    ASSERT_THAT(cc.get_links(owner, input), ContainerEq(expected));
}

TEST(CompositionCompiler, IfElseIfElseConditionTest) {
    auto cc = CompositionCompiler{"10.10.10.1_java:C:S1"};
    std::string cond =  R"(if(10.10.10.1_java:C:S1=="FOO"){)"
                        R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S2;)"
                        R"(}elseif(10.10.10.1_java:C:S1=="BOO"){)"
                        R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S3;)"
                        R"(}else{)"
                        R"(  10.10.10.1_java:C:S1+10.10.10.1_java:C:S4;)"
                        R"(})";

    cc.compile(cond);
    auto owner = ServiceState("10.10.10.1_java:C:S1", "FOO");
    auto input = ServiceState("WHATEVER", "DON'T CARE");

    auto expected = output_set{"10.10.10.1_java:C:S2"};

    ASSERT_THAT(cc.get_links(owner, input), ContainerEq(expected));
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
