#include <doctest/doctest.h>

#include "../../src/common/utils.hxx"
#include "../test_utilities.hxx"

namespace clubmoss::test {

TEST_SUITE("Test Utils") {

    static constexpr auto SUB_PATH{"sub_path/dummy.type"};

    TEST_CASE("test Utils::absPath(str)") {
        REQUIRE_NOTHROW(Utils::absPath(SUB_PATH));
    }

    TEST_CASE("show Utils::absPath(str) I/O") {
        printTitle("Show Utils::absPath(str) I/O:");
        const std::string abs_path = Utils::absPath(SUB_PATH);
        fmt::println(stderr, "i - relative path: {}", SUB_PATH);
        fmt::println(stderr, "o - absolute path: {}", abs_path);
        blankLine();
    }
}

}
