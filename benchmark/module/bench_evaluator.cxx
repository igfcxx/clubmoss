#include <omp.h>
#include <thread>

#include <fmt/core.h>
#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/module/evaluator/evaluator.hxx"
#include "../../src/layout/layout_manager.hxx"

static constexpr size_t NUM_SAMPLES = 1000;
static constexpr size_t MAX_THREADS = 12;

namespace clubmoss::evaluator::bench {

TEST_SUITE("Bench multi-threading Evaluator::evaluate()") {

    using Samples = std::vector<std::unique_ptr<Sample>>;

    layout::Manager manager;
    Evaluator evaluator;

    auto createSamples(layout::Manager& manager) -> Samples {
        Samples samples;
        for (uz i = 0; i < NUM_SAMPLES; ++i) {
            samples.emplace_back(std::make_unique<Sample>(manager.create()));
        }
        return samples;
    }

    auto baseline(ankerl::nanobench::Bench& bench) -> void {
        const Samples samples = createSamples(manager);
        bench.run(
            "baseline",
            [&]() -> void {
                for (uz i = 0; i < NUM_SAMPLES; ++i) {
                    evaluator.evaluate(*samples[i]);
                }
            }
        );
    }

    auto benchOmpGuided(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        const Samples samples = createSamples(manager);
        bench.run(
            fmt::format("omp: guided ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(guided) shared(samples) firstprivate(evaluator) lastprivate(evaluator) default (none)
                for (uz i = 0; i < NUM_SAMPLES; ++i) {
                    evaluator.evaluate(*samples[i]);
                }
            }
        );
    }

    TEST_CASE("bench Evaluator::evaluate()") {
        REQUIRE_LE(MAX_THREADS, std::thread::hardware_concurrency());
        REQUIRE_GE(MAX_THREADS, 1);

        ankerl::nanobench::Bench b;

        b.title("Evaluator::evaluate()")
         .unit("sample processed")
         .relative(true)
         .warmup(5)
         .minEpochIterations(100);
        b.performanceCounters(true);

        baseline(b);
        for (uz i = 2; i <= MAX_THREADS; ++i) {
            omp_set_num_threads(static_cast<int>(i));
            benchOmpGuided(b, i);
        }
    }
}

}
