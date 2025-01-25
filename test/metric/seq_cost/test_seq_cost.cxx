#include <doctest/doctest.h>

#include "../../../src/metric/seq_cost/seq_cost.hxx"
#include "../../../src/layout/layout_manager.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::metric::seq_cost::test {

TEST_CASE("test metric::SeqCost") {
    static const std::string D_PATH = Utils::absPath("test/metric/seq_cost/data.toml");
    Data data(toml::parse<toml::ordered_type_config>(D_PATH));
    SeqCost kc_metric(std::move(data));
    layout::Manager manager;

    SUBCASE("load config") {
        static const Toml M_CFG = toml::parse(Utils::absPath("test/metric/metric.toml"));
        static const Toml S_CFG = toml::parse(Utils::absPath("test/metric/score.toml"));
        Config::loadCfg(M_CFG, S_CFG);
    }

    SUBCASE("measure() and analyze()") {
        const fz cost_q1 = kc_metric.measure(layout::baselines::QWERTY);
        const fz cost_d1 = kc_metric.measure(layout::baselines::DVORAK);
        CHECK(cost_q1 > cost_d1);

        const fz cost_q2 = kc_metric.measure(layout::baselines::QWERTY);
        const fz cost_d2 = kc_metric.measure(layout::baselines::DVORAK);
        CHECK(cost_q2 == doctest::Approx(cost_q1).epsilon(0.01));
        CHECK(cost_d2 == doctest::Approx(cost_d1).epsilon(0.01));

        const fz cost_q3 = kc_metric.analyze(layout::baselines::QWERTY);
        const fz cost_d3 = kc_metric.analyze(layout::baselines::DVORAK);
        CHECK(cost_q3 == doctest::Approx(cost_q1).epsilon(0.01));
        CHECK(cost_d3 == doctest::Approx(cost_d1).epsilon(0.01));
    }

    SUBCASE("show costs of random layouts") {
        printTitle("Show metric::SeqCost results - random layouts:");
        for (uz i = 1; i <= 5; i++) {
            const Layout layout = manager.create();
            const std::string s = layout.toString();
            const fz cost = kc_metric.measure(layout);
            fmt::println(stderr, "{:d}. {:s} - {:.3f}", i, s, cost);
        }
        blankLine();
    }

    SUBCASE("show costs of baselines") {
        printTitle("Show metric::SeqCost results - baselines:");
        for (const auto& [i, layout] : layout::baselines::ALL | std::views::enumerate) {
            const std::string s = layout.name;
            const fz cost = kc_metric.measure(layout);
            fmt::println(stderr, "{:0>2d}. {:10s} {:.3f}", i + 1, s, cost);
        }
        blankLine();
    }
}

}
