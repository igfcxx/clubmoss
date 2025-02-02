#include <omp.h>
#include <thread>

#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/module/evaluator/evaluator.hxx"

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

    auto evalMeasure(ankerl::nanobench::Bench& bench) -> void {
        const Samples samples = createSamples(manager);
        bench.run(
            "measure() - baseline",
            [&]() -> void {
                for (uz i = 0; i < NUM_SAMPLES; ++i) {
                    evaluator.measure(*samples[i]);
                }
            }
        );
    }

    auto evalAnalyze(ankerl::nanobench::Bench& bench) -> void {
        const Samples samples = createSamples(manager);
        bench.run(
            "analyze() - baseline",
            [&]() -> void {
                for (uz i = 0; i < NUM_SAMPLES; ++i) {
                    evaluator.analyze(*samples[i]);
                }
            }
        );
    }

    auto evalMeasureMt(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        const Samples samples = createSamples(manager);
        bench.run(
            fmt::format("measure() - {: >2d} threads", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(guided) shared(samples) firstprivate(evaluator) lastprivate(evaluator) default (none)
                for (uz i = 0; i < NUM_SAMPLES; ++i) {
                    evaluator.measure(*samples[i]);
                }
            }
        );
    }

    auto evalAnalyzeMt(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        const Samples samples = createSamples(manager);
        bench.run(
            fmt::format("analyze() - {: >2d} threads", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(guided) shared(samples) firstprivate(evaluator) lastprivate(evaluator) default (none)
                for (uz i = 0; i < NUM_SAMPLES; ++i) {
                    evaluator.analyze(*samples[i]);
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

        evalMeasure(b);
        for (uz i = 2; i <= MAX_THREADS; ++i) {
            omp_set_num_threads(static_cast<int>(i));
            evalMeasureMt(b, i);
        }

        evalAnalyze(b);
        for (uz i = 2; i <= MAX_THREADS; ++i) {
            omp_set_num_threads(static_cast<int>(i));
            evalAnalyzeMt(b, i);
        }
    }
}

}
