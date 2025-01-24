#include <doctest/doctest.h>

#include "../../../src/module/evaluator/evaluator.hxx"
#include "../../../src/layout/layout_manager.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::evaluator::test {

TEST_SUITE("Test Evaluator") {

    TEST_CASE("test Evaluator() construction") {
        REQUIRE_NOTHROW(Evaluator());
    }

    layout::Manager manager;
    Evaluator evaluator;

    TEST_CASE("test Evaluator::evaluate(Layout)") {
        Sample s1(manager.create());
        Sample s2(manager.create());
        REQUIRE_NE(s1, s2);

        evaluator.evaluate(s1);
        evaluator.evaluate(s2);
        CHECK_NE(s1.getLoss(), s2.getLoss());
    }

    TEST_CASE("show Sample losses") {

        SUBCASE("random layouts") {
            printTitle("Show Sample losses - random layouts:");
            for (uz i = 1; i <= 5; i++) {
                Sample sample(manager.create());
                evaluator.evaluate(sample);
                fmt::println(
                    stderr, "{:d}. {:s} - {:.3f}",
                    i, sample.toString(), sample.getLoss()
                );
            }
            blankLine();
        }

        SUBCASE("baselines") {
            printTitle("Show Sample losses - baselines:");
            for (const auto& [i, layout] : layout::baselines::ALL | std::views::enumerate) {
                Sample sample(layout);
                evaluator.evaluate(sample);
                fmt::println(
                    stderr, "{:0>2d}. {:10s} {:.3f}",
                    i + 1, layout.name, sample.getLoss()
                );
            }
            blankLine();
        }
    }
}

}
