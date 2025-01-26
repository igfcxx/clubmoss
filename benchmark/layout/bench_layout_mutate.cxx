#include <omp.h>
#include <thread>

#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/layout/layout_manager.hxx"
#include "default_layout_configs.hxx"

static constexpr size_t NUM_LAYOUTS = 1000;
static constexpr size_t MAX_THREADS = 4;

namespace clubmoss::layout::bench::mutate {

TEST_SUITE("Bench multi-threading layout::Manager::mutate()") {

    using LytVec = std::vector<std::unique_ptr<Layout>>;

    Manager manager;

    auto checkRandomness(LytVec& v) -> void {
        std::ranges::sort(v);
        uz num_duplicates = 0;
        for (uz i = 0; i < v.size() - 1; ++i) {
            if (v[i] == v[i + 1]) {
                ++num_duplicates;
            }
        }
        CHECK_LT(num_duplicates, NUM_LAYOUTS / 100);
    }

    auto createLayouts(Manager& manager) -> LytVec {
        LytVec layouts;
        for (uz i = 0; i < NUM_LAYOUTS * 2; ++i) {
            layouts.emplace_back(std::make_unique<Layout>(manager.create()));
        }
        return layouts;
    }

    auto baseline(ankerl::nanobench::Bench& bench) -> void {
        LytVec layouts = createLayouts(manager);
        bench.run(
            "baseline (1)",
            [&]() -> void {
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.mutate(*layouts[i + NUM_LAYOUTS], *layouts[i]);
                }
            }
        );
        checkRandomness(layouts);
    }

    auto benchOmpStatic(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        LytVec layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: static ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(static) shared(layouts) firstprivate(manager) lastprivate(manager) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.mutate(*layouts[i + NUM_LAYOUTS], *layouts[i]);
                }
            }
        );
        checkRandomness(layouts);
    }

    auto benchOmpGuided(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        LytVec layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: guided ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(guided) shared(layouts) firstprivate(manager) lastprivate(manager) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.mutate(*layouts[i + NUM_LAYOUTS], *layouts[i]);
                }
            }
        );
        checkRandomness(layouts);
    }

    auto benchStdThread(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        std::vector managers(num_threads, Manager());
        LytVec layouts = createLayouts(managers[0]);

        auto process_range = [](Manager& manager, const LytVec& layouts, const uz beg, const uz end) {
            for (uz i = beg; i < end; ++i) { manager.mutate(*layouts[i + NUM_LAYOUTS], *layouts[i]); }
        };

        const uz range = NUM_LAYOUTS / num_threads;
        bench.run(
            fmt::format("std::thread ({:d})", num_threads).c_str(),
            [&]() -> void {
                std::vector<std::thread> threads{};
                for (uz thread_id = 0; thread_id < num_threads; ++thread_id) {
                    const uz beg = thread_id * range;
                    const uz end = (thread_id == num_threads - 1) ? NUM_LAYOUTS : beg + range;
                    threads.emplace_back(process_range, std::ref(managers[thread_id]), std::ref(layouts), beg, end);
                }
                for (auto& t : threads) {
                    t.join();
                }
                threads.clear();
            }
        );
        checkRandomness(layouts);
    }

    auto benchAll(std::string_view title) -> void {
        ankerl::nanobench::Bench b;

        b.title(fmt::format("mutate() - {:s}", title))
         .unit("layout processed")
         .relative(true)
         .warmup(10)
         .minEpochIterations(500);
        b.performanceCounters(true);

        baseline(b);
        for (uz i = 2; i <= MAX_THREADS; ++i) {
            omp_set_num_threads(static_cast<int>(i));
            // benchStdThread(b, i);
            // benchOmpStatic(b, i);
            benchOmpGuided(b, i);
        }
    }

    TEST_CASE("bench layout::Manager::mutate()") {
        REQUIRE_LE(MAX_THREADS, std::thread::hardware_concurrency());
        REQUIRE_GE(MAX_THREADS, 1);

        Manager::loadCfg(CFG_1_AREA);
        benchAll("1 area"); // 26 可变按键
        Manager::loadCfg(CFG_2_AREA);
        benchAll("2 area"); // 22 + 4 可变按键
        Manager::loadCfg(CFG_3_AREA);
        benchAll("3 area"); // 14 + 8 + 4 可变按键
    }
}

}
