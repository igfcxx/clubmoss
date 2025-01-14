#include <omp.h>
#include <thread>

#include <fmt/core.h>
#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/metric/dis_cost/dis_cost.hxx"
#include "../../src/layout/layout_manager.hxx"

static constexpr size_t NUM_LAYOUTS = 1000;
static constexpr size_t MAX_THREADS = 12;

namespace clubmoss::metric::dis_cost::bench {

TEST_SUITE("Bench multi-threading metric::DisCost::measure()") {

    using LytVec = std::vector<std::unique_ptr<Layout>>;

    static const std::string PATH = Utils::absPath("test/metric/dis_cost/data.toml");
    const Data EN_DATA(toml::parse(PATH));

    DisCost dc_metric(EN_DATA);
    layout::Manager manager;

    auto createLayouts(layout::Manager& manager) -> LytVec {
        LytVec layouts;
        for (uz i = 0; i < NUM_LAYOUTS; ++i) {
            layouts.emplace_back(std::make_unique<Layout>(manager.create()));
        }
        return layouts;
    }

    auto baseline1(ankerl::nanobench::Bench& bench) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            "baseline1 (1)",
            [&]() -> void {
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    dc_metric.measure1(*layouts[i]);
                }
            }
        );
    }

    auto baseline2(ankerl::nanobench::Bench& bench) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            "baseline2 (1)",
            [&]() -> void {
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    dc_metric.measure2(*layouts[i]);
                }
            }
        );
    }

    auto baseline3(ankerl::nanobench::Bench& bench) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            "baseline3 (1)",
            [&]() -> void {
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    dc_metric.measure3(*layouts[i]);
                }
            }
        );
    }

    auto benchOmpStatic(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: static ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(static) shared(layouts) firstprivate(dc_metric) lastprivate(dc_metric) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    dc_metric.measure1(*layouts[i]);
                }
            }
        );
    }

    auto benchOmpGuided(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        const LytVec layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: guided ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(guided) shared(layouts) firstprivate(dc_metric) lastprivate(dc_metric) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    dc_metric.measure1(*layouts[i]);
                }
            }
        );
    }

    TEST_CASE("bench metric::DisCost::measure()") {
        REQUIRE_LE(MAX_THREADS, std::thread::hardware_concurrency());
        REQUIRE_GE(MAX_THREADS, 1);

        ankerl::nanobench::Bench b;

        b.title("DisCost::measure()")
         .unit("layout processed")
         .relative(true)
         .warmup(5)
         .minEpochIterations(100);
        b.performanceCounters(true);

        baseline1(b);
        // baseline2(b);
        // baseline3(b);

        for (uz i = 2; i <= MAX_THREADS; ++i) {
            omp_set_num_threads(static_cast<int>(i));
            benchOmpStatic(b, i);
            benchOmpGuided(b, i);
        }
    }
}

}
