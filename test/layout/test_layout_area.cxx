#include <doctest/doctest.h>

#include "../../src/layout/layout_area.hxx"
#include "../test_utilities.hxx"

namespace clubmoss::layout::test {

TEST_SUITE("Test layout::Area") {

    static const Layout QWERTY = baselines::QWERTY;
    static const auto CONFIG   = u8R"(
        cap_list = ["Q", "W", "E"]
        pos_list = [0, 1, 2]
    )"_toml;

    TEST_CASE("test layout::Area(toml) construction") {
        REQUIRE_NOTHROW((Area(CONFIG)));
    }

    static auto area = Area(CONFIG);
    static auto prng = Prng();

    TEST_CASE("test layout::Area::assign()") {

        // 对于一个由 CONFIG 定义的可变区域, 其 assign(QWERTY) 共有 6 种可能的输出,
        // 其中有一种与 QWERTY 完全相同. 由于这些事件的发生概率(理论上)相等,
        // 因此 P(area.assign() == QWERTY) 应该为 1/6.

        static constexpr uz TOLERANCE   = 20;
        static constexpr uz EXPECTATION = 100;
        static constexpr uz TEST_EPOCHS = EXPECTATION * 6;

        Layout layout(QWERTY);
        uz counter = 0;

        for (uz i = 0; i < TEST_EPOCHS; ++i) {
            area.assign(layout, prng);
            if (layout == QWERTY) {
                ++counter;
            }
        }

        CHECK_GT(counter, EXPECTATION - TOLERANCE);
        CHECK_LT(counter, EXPECTATION + TOLERANCE);
    }

    TEST_CASE("test layout::Area::mutate()") {
        Layout prev(QWERTY), curr = prev;
        for (uz i = 0; i < 10; ++i) {
            prev = curr;
            area.mutate(curr, prng);
            CHECK_EQ(munOfDiffKeys(prev, curr), 2);
        }
    }
}

}
