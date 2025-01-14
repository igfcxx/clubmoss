#include <doctest/doctest.h>

#include "../../../src/metric/key_cost/key_cost.hxx"
#include "../../../src/layout/layout_manager.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::metric::key_cost::test {

TEST_SUITE("Test metric::KeyCost") {

    static const std::string DATA_PATH = Utils::absPath("test/metric/key_cost/data.toml");
    static const std::string CFG_PATH  = Utils::absPath("test/metric/key_cost/cfg.toml");

    const Data EN_DATA(toml::parse(DATA_PATH));

    TEST_CASE("test metric::KeyCost(Toml) construction") {
        KeyCost::loadCfg(toml::parse(CFG_PATH).at("key_cost"));
        REQUIRE_NOTHROW((KeyCost(EN_DATA)));
    }

    KeyCost kc_metric(EN_DATA);
    layout::Manager manager;

    TEST_CASE("test metric::KeyCost::measure()") {
        const fz cost_1 = kc_metric.measure1(layout::baselines::QWERTY);
        const fz cost_2 = kc_metric.measure1(layout::baselines::DVORAK);
        CHECK(cost_1 > cost_2);
        CHECK_LE(std::abs(kc_metric.measure1(layout::baselines::QWERTY) - kc_metric.measure2(layout::baselines::QWERTY)), 1e-5);
        CHECK_LE(std::abs(kc_metric.measure1(layout::baselines::DVORAK) - kc_metric.measure2(layout::baselines::DVORAK)), 1e-5);
        CHECK_LE(std::abs(kc_metric.measure2(layout::baselines::QWERTY) - kc_metric.measure3(layout::baselines::QWERTY)), 1e-5);
        CHECK_LE(std::abs(kc_metric.measure2(layout::baselines::DVORAK) - kc_metric.measure3(layout::baselines::DVORAK)), 1e-5);
    }

    TEST_CASE("test metric::KeyCost::check()") {
        CHECK_FALSE(kc_metric.check(layout::baselines::QWERTY));
        CHECK_FALSE(kc_metric.check(layout::baselines::DVORAK));
        CHECK(kc_metric.check(layout::baselines::NORMAN));
    }

    TEST_CASE("show metric::KeyCost scores - random layouts") {
        printTitle("Show metric::KeyCost scores - random layouts:");
        for (uz i = 1; i <= 5; i++) {
            const Layout layout = manager.create();
            const std::string s = layout.toCapSeq();
            const fz cost       = kc_metric.measure1(layout);
            fmt::println(stderr, "{:d}. {:s} - {:.3f}", i, s, cost);
        }
        blankLine();
    }

    TEST_CASE("show metric::KeyCost scores - baselines") {
        printTitle("Show metric::KeyCost scores - baselines:");
        for (const auto& [i, layout] : layout::baselines::ALL | std::views::enumerate) {
            const std::string s = layout.name;
            const fz cost       = kc_metric.measure1(layout);
            fmt::println(stderr, "{:0>2d}. {:10s} - {:.3f}", i + 1, s, cost);
        }
        blankLine();
    }
}

}
