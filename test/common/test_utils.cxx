#include <doctest/doctest.h>

#include "../../src/common/utils.hxx"
#include "../utilities.hxx"

namespace clubmoss::test {

TEST_SUITE("Test Utils") {

    static constexpr auto SUB_PATH{"sub_path/dummy.type"};

    TEST_CASE("test Utils::absPath(str)") {
        REQUIRE_NOTHROW(Utils::absPath(SUB_PATH));
    }

    TEST_CASE("show Utils::absPath(str) I/O") {
        const std::string abs_path = Utils::absPath(SUB_PATH);
        fmt::println("\nShow Utils::absPath(str) I/O:");
        fmt::println("i - relative path: {}", SUB_PATH);
        fmt::println("o - absolute path: {}", abs_path);
        fmt::print("\n");
    }
}

}
