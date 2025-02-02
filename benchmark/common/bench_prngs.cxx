#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/common/types.hxx"

namespace clubmoss::prng::bench {

TEST_SUITE("Bench PRNGs") {

    template <typename Prng> auto bench(
        ankerl::nanobench::Bench& b,
        const std::string_view name
    ) -> void {
        Prng prng(std::random_device{}());
        std::vector<uz> data{0, 1, 2, 3, 4, 5, 6, 7};
        b.run(
            name.data(), [&]() -> void { std::ranges::shuffle(data, prng); }
        );
    }

    TEST_CASE("bench PRNGs with u64 output") {
        ankerl::nanobench::Bench b;
        b.title("PRNGs (64-bit)")
         .unit("std::shuffle()")
         .relative(true)
         .warmup(10)
         .minEpochIterations(50000);
        b.performanceCounters(true);

        bench<std::mt19937_64>(b, "std::mt19937"); // baseline
        bench<SplitMix64>(b, "SplitMix");
        bench<RomuTrio64>(b, "RomuTrio");
        bench<WyRand64>(b, "WyRand");
    }
}

}
