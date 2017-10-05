#include "../src/composition.hpp"

using namespace clara::composition;

int main() {
    std::string composition = R"(10.10.10.1_java:C:S1+)"
            R"(10.10.10.1_java:C:S2+)"
            R"(10.10.10.1_java:C:S3+)"
            R"(10.10.10.1_java:C:S4;)";

    auto cc = CompositionCompiler{"10.10.10.1_java:C:S3"};

    cc.compile(composition);
}
