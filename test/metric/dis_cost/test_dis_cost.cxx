#include <doctest/doctest.h>

#include "../../../src/metric/dis_cost/dis_cost.hxx"
#include "../../../src/layout/layout_manager.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::metric::dis_cost::test {

TEST_SUITE("Test metric::DisCost") {

    static const std::string DATA_PATH = Utils::absPath("test/metric/dis_cost/data.toml");
    static const std::string CFG_PATH = Utils::absPath("test/metric/dis_cost/cfg.toml");

    TEST_CASE("test metric::DisCost(Toml) construction") {
        DisCost::loadCfg(toml::parse(CFG_PATH).at("dis_cost"));
        const Data data(toml::parse<toml::ordered_type_config>(DATA_PATH));
        REQUIRE_NOTHROW((DisCost(data)));
    }

    const Data EN_DATA(toml::parse<toml::ordered_type_config>(DATA_PATH));
    DisCost dc_metric(EN_DATA);
    layout::Manager manager;

    TEST_CASE("test metric::DisCost::measure()") {
        const fz cost_1 = dc_metric.measure(layout::baselines::QWERTY);
        const fz cost_2 = dc_metric.measure(layout::baselines::DVORAK);
        CHECK(cost_1 > cost_2);
    }

    TEST_CASE("test metric::DisCost::check()") {
        CHECK_FALSE(dc_metric.check(layout::baselines::QWERTY));
        CHECK_FALSE(dc_metric.check(layout::baselines::DVORAK));
        CHECK(dc_metric.check(layout::baselines::NORMAN));
    }

    TEST_CASE("show metric::DisCost scores") {

        SUBCASE("random layouts") {
            printTitle("Show metric::DisCost scores - random layouts:");
            for (uz i = 1; i <= 5; i++) {
                const Layout layout = manager.create();
                const std::string s = layout.toCapSeq();
                const fz cost = dc_metric.measure(layout);
                fmt::println(stderr, "{:d}. {:s} - {:.3f}", i, s, cost);
            }
            blankLine();
        }

        SUBCASE("baselines") {
            printTitle("Show metric::DisCost scores - baselines:");
            for (const auto& [i, layout] : layout::baselines::ALL | std::views::enumerate) {
                const std::string s = layout.name;
                const fz cost = dc_metric.measure(layout);
                fmt::println(stderr, "{:0>2d}. {:10s} {:.3f}", i + 1, s, cost);
            }
            blankLine();
        }
    }
}

}
