#include <doctest/doctest.h>

#include "../../../src/metric/seq_cost/seq_cost.hxx"
#include "../../../src/layout/layout_manager.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::metric::seq_cost::test {

TEST_SUITE("Test metric::SeqCost") {

    static const std::string DATA_PATH = Utils::absPath("test/metric/seq_cost/data.toml");
    static const std::string CFG_PATH = Utils::absPath("test/metric/seq_cost/cfg.toml");

    const Data data(toml::parse<toml::ordered_type_config>(DATA_PATH));
    layout::Manager manager;
    SeqCost sc_metric;

    TEST_CASE("test metric::SeqCost::measure()") {
        SeqCost::loadCfg(toml::parse(CFG_PATH).at("seq_cost"));
        const fz cost_1 = sc_metric.measure(layout::baselines::QWERTY, data);
        const fz cost_2 = sc_metric.measure(layout::baselines::DVORAK, data);
        CHECK(cost_1 > cost_2);
    }

    TEST_CASE("test metric::SeqCost::check()") {
        CHECK_FALSE(sc_metric.check(layout::baselines::QWERTY, data));
        CHECK(sc_metric.check(layout::baselines::DVORAK, data));
        CHECK(sc_metric.check(layout::baselines::NORMAN, data));
    }

    TEST_CASE("show metric::SeqCost scores") {

        SUBCASE("random layouts") {
            printTitle("Show metric::SeqCost scores - random layouts:");
            for (uz i = 1; i <= 5; i++) {
                const Layout layout = manager.create();
                const std::string s = layout.toString();
                const fz cost = sc_metric.measure(layout, data);
                fmt::println(stderr, "{:d}. {:s} - {:.3f}", i, s, cost);
            }
            blankLine();
        }

        SUBCASE("baselines") {
            printTitle("Show metric::SeqCost scores - baselines:");
            for (const auto& [i, layout] : layout::baselines::ALL | std::views::enumerate) {
                const std::string s = layout.name;
                const fz cost = sc_metric.measure(layout, data);
                fmt::println(stderr, "{:0>2d}. {:10s} {:.3f}", i + 1, s, cost);
            }
            blankLine();
        }
    }
}

}
