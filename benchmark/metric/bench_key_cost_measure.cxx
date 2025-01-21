#include <omp.h>
#include <thread>

#include <fmt/core.h>
#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/metric/key_cost/key_cost.hxx"
#include "../../src/layout/layout_manager.hxx"

static constexpr size_t NUM_LAYOUTS = 1000;
static constexpr size_t MAX_THREADS = 8;

namespace clubmoss::metric::key_cost::bench {

TEST_SUITE("Bench multi-threading metric::KeyCost::measure()") {

    using LytVec = std::vector<std::unique_ptr<Layout>>;

    static const std::string PATH = Utils::absPath("test/metric/key_cost/data.toml");
    const Data EN_DATA(toml::parse<toml::ordered_type_config>(PATH));

    KeyCost kc_metric(EN_DATA);
    layout::Manager manager;

    auto createLayouts(layout::Manager& manager) -> LytVec {
        LytVec layouts;
        for (uz i = 0; i < NUM_LAYOUTS; ++i) {
            layouts.emplace_back(std::make_unique<Layout>(manager.create()));
        }
        return layouts;
    }

    auto baseline(ankerl::nanobench::Bench& bench) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            "baseline",
            [&]() -> void {
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    kc_metric.measure(*layouts[i]);
                }
            }
        );
    }

    auto benchOmpStatic(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: static ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(static) shared(layouts) firstprivate(kc_metric) lastprivate(kc_metric) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    kc_metric.measure(*layouts[i]);
                }
            }
        );
    }

    auto benchOmpGuided(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: guided ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(guided) shared(layouts) firstprivate(kc_metric) lastprivate(kc_metric) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    kc_metric.measure(*layouts[i]);
                }
            }
        );
    }

    TEST_CASE("bench metric::KeyCost::measure()") {
        REQUIRE_LE(MAX_THREADS, std::thread::hardware_concurrency());
        REQUIRE_GE(MAX_THREADS, 1);

        ankerl::nanobench::Bench b;

        b.title("KeyCost::measure()")
         .unit("layout processed")
         .relative(true)
         .warmup(5)
         .minEpochIterations(200);
        b.performanceCounters(true);

        baseline(b);
        for (uz i = 2; i <= MAX_THREADS; ++i) {
            omp_set_num_threads(static_cast<int>(i));
            // benchOmpStatic(b, i);
            benchOmpGuided(b, i);
        }
    }
}

}
