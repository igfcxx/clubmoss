#include <doctest/doctest.h>

#include "../../../src/module/evaluator/analyzer.hxx"
#include "../../../src/layout/layout_manager.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::analyzer::test {

TEST_SUITE("Test Analyzer") {

    TEST_CASE("test Analyzer() construction") {
        REQUIRE_NOTHROW(Analyzer());
    }

    layout::Manager manager;
    Analyzer analyzer;

    TEST_CASE("test Analyzer::analyze(Layout)") {
        Sample s1(manager.create());
        Sample s2(manager.create());
        REQUIRE_NE(s1, s2);

        analyzer.analyze(s1);
        analyzer.analyze(s2);
        CHECK_NE(s1.getLoss(), s2.getLoss());
    }

    TEST_CASE("show Sample stats") {

        SUBCASE("random layouts") {
            printTitle("Show Sample losses - random layouts:");
            for (uz i = 0; i < 3; i++) {
                Sample sample(manager.create());
                const std::string stats = analyzer.analyze(sample);
                fmt::println(stderr, "{:s}", stats);
            }
            blankLine();
        }

        SUBCASE("baselines") {
            printTitle("Show Sample losses - baselines:");
            for (const auto& [i, layout] : layout::baselines::ALL | std::views::enumerate) {
                Sample sample(layout);
                const std::string stats = analyzer.analyze(sample);
                fmt::println(stderr, "{:s}", stats);
            }
            blankLine();
        }
    }
}

}
