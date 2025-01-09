#include <omp.h>
#include <thread>

#include <fmt/core.h>
#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/layout/layout_manager.hxx"

static constexpr size_t NUM_LAYOUTS    = 1000;
static constexpr size_t MAX_DUPLICATES = 5;
static constexpr size_t MAX_THREADS    = 8;

namespace clubmoss::layout::bench::reinit {

TEST_SUITE("Bench multi-threading layout::Manager::reinit()") {

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
        CHECK_LT(num_duplicates, MAX_DUPLICATES);
    }

    auto createLayouts(Manager& manager) -> LytVec {
        LytVec layouts;
        for (uz i = 0; i < NUM_LAYOUTS; ++i) {
            layouts.emplace_back(std::make_unique<Layout>(manager.create()));
        }
        return layouts;
    }

    auto baseline(ankerl::nanobench::Bench& bench) -> void {
        auto layouts = createLayouts(manager);
        bench.run(
            "baseline (1)",
            [&]() -> void {
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.reinit(*layouts[i]);
                }
            }
        );
        checkRandomness(layouts);
    }

    auto benchOmpStatic(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        auto layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: static ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(static) shared(layouts) firstprivate(manager) lastprivate(manager) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.reinit(*layouts[i]);
                }
            }
        );
        checkRandomness(layouts);
    }

    auto benchOmpGuided(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        auto layouts = createLayouts(manager);
        bench.run(
            fmt::format("omp: guided ({:d})", num_threads).c_str(),
            [&]() -> void {
                #pragma omp parallel for schedule(guided) shared(layouts) firstprivate(manager) lastprivate(manager) default (none)
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.reinit(*layouts[i]);
                }
            }
        );
        checkRandomness(layouts);
    }

    void processRange(Manager& manager, const LytVec& layouts, const uz beg, const uz end) {
        for (uz i = beg; i < end; ++i) {
            manager.reinit(*layouts[i]);
        }
    }

    auto benchStdThread(ankerl::nanobench::Bench& bench, const uz num_threads) -> void {
        std::vector managers(num_threads, Manager());
        auto layouts = createLayouts(managers[0]);

        const uz range = NUM_LAYOUTS / num_threads;
        bench.run(
            fmt::format("std::thread ({:d})", num_threads).c_str(),
            [&]() -> void {
                std::vector<std::thread> threads{};
                for (uz thread_id = 0; thread_id < num_threads; ++thread_id) {
                    const uz beg = thread_id * range;
                    const uz end = (thread_id == num_threads - 1) ? NUM_LAYOUTS : beg + range;
                    threads.emplace_back(processRange, std::ref(managers[thread_id]), std::ref(layouts), beg, end);
                }
                for (auto& t : threads) {
                    t.join();
                }
                threads.clear();
            }
        );
        checkRandomness(layouts);
    }

    auto bench(std::string_view pattern_name) -> void {
        ankerl::nanobench::Bench b;

        b.title(fmt::format("reinit() - {:s}", pattern_name))
         .unit("layout processed")
         .relative(true)
         .warmup(10)
         .minEpochIterations(500);
        b.performanceCounters(true);

        baseline(b);
        for (uz i = 2; i <= MAX_THREADS; ++i) {
            omp_set_num_threads(static_cast<int>(i));
            benchOmpStatic(b, i);
            benchOmpGuided(b, i);
            // benchStdThread(b, i);
        }
    }

    const auto cfg1 = u8R"(
        [[pinned_keys]]
        cap = ";"
        pos = 9

        [[pinned_keys]]
        cap = ","
        pos = 27

        [[pinned_keys]]
        cap = "."
        pos = 28

        [[pinned_keys]]
        cap = "/"
        pos = 29
    )"_toml;

    const auto cfg2 = u8R"(
        [[mutable_areas]]
        cap_list = ["Z", "X", "C", "V"]
        pos_list = [20, 21, 22, 23]

        [[pinned_keys]]
        cap = ";"
        pos = 9

        [[pinned_keys]]
        cap = ","
        pos = 27

        [[pinned_keys]]
        cap = "."
        pos = 28

        [[pinned_keys]]
        cap = "/"
        pos = 29
    )"_toml;

    const auto cfg3 = u8R"(
        [[mutable_areas]]
        cap_list = ["A", "E", "I", "O", "U", "N", "H", "T"]
        pos_list = [10, 11, 12, 13, 16, 17, 18, 19]

        [[mutable_areas]]
        cap_list = ["Z", "X", "C", "V"]
        pos_list = [20, 21, 22, 23]

        [[pinned_keys]]
        cap = ";"
        pos = 9

        [[pinned_keys]]
        cap = ","
        pos = 27

        [[pinned_keys]]
        cap = "."
        pos = 28

        [[pinned_keys]]
        cap = "/"
        pos = 29
    )"_toml;

    TEST_CASE("bench layout::Manager::reinit()") {
        REQUIRE_LE(MAX_THREADS, std::thread::hardware_concurrency());
        REQUIRE_GE(MAX_THREADS, 1);

        Manager::loadCfg(cfg1);
        bench("1 area"); // 26 mutable keys, 4 pinned keys
        Manager::loadCfg(cfg2);
        bench("2 area"); // 22 + 4 mutable keys, 4 pinned keys
        Manager::loadCfg(cfg2);
        bench("3 area"); // 14 + 8 + 4 mutable keys, 4 pinned keys
    }
}

}
