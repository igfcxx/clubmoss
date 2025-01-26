#include <omp.h>
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
        Sample a(manager.create());
        Sample b(manager.create());
        REQUIRE_NE(a, b);

        evaluator.measure(a);
        evaluator.measure(b);
        const fz loss_a1 = a.getLoss();
        const fz loss_b1 = b.getLoss();
        CHECK_NE(loss_a1, loss_b1);

        evaluator.analyze(a);
        evaluator.analyze(b);
        const fz loss_a2 = a.getLoss();
        const fz loss_b2 = b.getLoss();
        CHECK_NE(loss_a2, loss_b2);

        if (a.getFlaws() == 0) {
            CHECK_EQ(loss_a2, doctest::Approx(loss_a1).epsilon(0.01));
        } else {
            CHECK_GT(loss_a2, doctest::Approx(loss_a1).epsilon(0.01));
        }

        if (b.getFlaws() == 0) {
            CHECK_EQ(loss_b2, doctest::Approx(loss_b1).epsilon(0.01));
        } else {
            CHECK_GT(loss_b2, doctest::Approx(loss_b1).epsilon(0.01));
        }
    }

    TEST_CASE("test multi-threaded Evaluator::evaluate(Layout)") {

        std::vector<std::unique_ptr<Sample>> samples;
        for (uz i = 0; i < 1000; ++i) {
            samples.emplace_back(std::make_unique<Sample>(manager.create()));
        }

        SUBCASE("measure()") {
            std::set<fz> losses;
            for (uz threads = 1; threads <= 4; ++threads) {
                omp_set_num_threads(static_cast<int>(threads));
                #pragma omp parallel for schedule(guided) shared(samples) firstprivate(evaluator) lastprivate(evaluator) default (none)
                for (uz i = 0; i < 1000; ++i) {
                    evaluator.measure(*samples[i]);
                }
                std::ranges::sort(
                    samples,
                    [](const std::unique_ptr<Sample>& lhs, const std::unique_ptr<Sample>& rhs) {
                        return lhs->getLoss() < rhs->getLoss();
                    }
                );
                losses.insert(samples[0]->getLoss());
            }
            WARN_EQ(losses.size(), 1);
        }

        SUBCASE("analize()") {
            std::set<fz> losses;
            for (uz threads = 1; threads <= 4; ++threads) {
                omp_set_num_threads(static_cast<int>(threads));
                #pragma omp parallel for schedule(guided) shared(samples) firstprivate(evaluator) lastprivate(evaluator) default (none)
                for (uz i = 0; i < 1000; ++i) {
                    evaluator.analyze(*samples[i]);
                }
                std::ranges::sort(
                    samples,
                    [](const std::unique_ptr<Sample>& lhs, const std::unique_ptr<Sample>& rhs) {
                        return lhs->getLoss() < rhs->getLoss();
                    }
                );
                losses.insert(samples[0]->getLoss());
            }
            WARN_EQ(losses.size(), 1);
        }
    }

    TEST_CASE("show Sample losses") {

        SUBCASE("random layouts") {
            printTitle("Show Sample losses - random layouts:");
            for (uz i = 1; i <= 5; i++) {
                Sample sample(manager.create());
                evaluator.measure(sample);
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
                evaluator.measure(sample);
                const fz l1 = sample.getLoss();
                evaluator.analyze(sample);
                const fz l2 = sample.getLoss();
                fmt::println(
                    stderr, "{:0>2d}. {:10s} {:.3f} - {:.3f}",
                    i + 1, layout.name, l1, l2
                );
            }
            blankLine();
        }
    }
}

}
