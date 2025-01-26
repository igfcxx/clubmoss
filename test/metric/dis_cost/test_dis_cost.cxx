#include <doctest/doctest.h>

#include "../../../src/metric/dis_cost/dis_cost.hxx"
#include "../../../src/layout/layout_manager.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::metric::dis_cost::test {

TEST_CASE("test metric::DisCost") {

    static const std::string D_PATH = Utils::absPath("test/metric/dis_cost/data.toml");
    Data data(toml::parse<toml::ordered_type_config>(D_PATH));
    DisCost metric(std::move(data));
    layout::Manager manager;

    SUBCASE("load config") {
        static const Toml M_CFG = toml::parse(Utils::absPath("test/metric/metric.toml"));
        static const Toml S_CFG = toml::parse(Utils::absPath("test/metric/score.toml"));
        Config::loadCfg(M_CFG, S_CFG);
    }

    SUBCASE("measure(), analyze() and ") {
        const fz cost_q1 = metric.measure(layout::baselines::QWERTY);
        const fz cost_d1 = metric.measure(layout::baselines::NORMAN);
        CHECK(cost_q1 > cost_d1);

        const fz cost_q2 = metric.measure(layout::baselines::QWERTY);
        const fz cost_d2 = metric.measure(layout::baselines::NORMAN);
        CHECK(cost_q2 == doctest::Approx(cost_q1).epsilon(0.01));
        CHECK(cost_d2 == doctest::Approx(cost_d1).epsilon(0.01));

        const auto [cost_q3, flaws_q] = metric.analyze(layout::baselines::QWERTY);
        const auto [cost_d3, flaws_d] = metric.analyze(layout::baselines::NORMAN);
        CHECK(cost_q3 == doctest::Approx(cost_q1).epsilon(0.01));
        CHECK(cost_d3 == doctest::Approx(cost_d1).epsilon(0.01));
        CHECK_GT(flaws_q, flaws_d);
    }

    SUBCASE("show costs of random layouts") {
        printTitle("Show metric::DisCost results - random layouts:");
        for (uz i = 1; i <= 5; i++) {
            const Layout layout = manager.create();
            const std::string s = layout.toString();
            const fz cost = metric.measure(layout);
            fmt::println(stderr, "{:d}. {:s} - {:.3f}", i, s, cost);
        }
        blankLine();
    }

    SUBCASE("show costs of baselines") {
        printTitle("Show metric::DisCost results - baselines:");
        for (const auto& [i, layout] : layout::baselines::ALL | std::views::enumerate) {
            const std::string s = layout.name;
            const fz cost = metric.measure(layout);
            fmt::println(stderr, "{:0>2d}. {:10s} {:.3f}", i + 1, s, cost);
        }
        blankLine();
    }
}

}
