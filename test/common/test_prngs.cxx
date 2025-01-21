#include <map>
#include <ranges>
#include <algorithm>
#include <doctest/doctest.h>

#include "../../src/common/types.hxx"

namespace clubmoss::prng::tests {

TEST_SUITE("Test PRNGs") {

    template <typename T>concept RandNumGenerator = requires(T engine) {
        { engine() } -> std::convertible_to<typename T::result_type>;
        { engine.min() } -> std::same_as<typename T::result_type>;
        { engine.max() } -> std::same_as<typename T::result_type>;
    };

    template <RandNumGenerator RNG> auto testShuffleQuality() -> bool {
        using Target = std::vector<uz>;

        static constexpr int NUM_PERMUTATIONS = 24;
        static constexpr int NUM_EXPERIMENTS  = 24'000;
        static constexpr int EXPECTED_FREQ    = NUM_EXPERIMENTS / NUM_PERMUTATIONS;

        RNG rng{std::random_device()()};
        std::map<Target, int> counter;
        Target s{0, 1, 2, 3};
        do {
            counter[s] = 0;
        } while (std::ranges::next_permutation(s).found);

        Target t{0, 1, 2, 3};
        for (uz i = 0; i < NUM_EXPERIMENTS; ++i) {
            std::ranges::shuffle(t, rng);
            counter[t]++;
        }

        // 计算卡方统计量
        double chi_square = 0.0;
        for (const int observed_freq : std::views::values(counter)) {
            const auto diff = static_cast<double>(observed_freq - EXPECTED_FREQ);
            chi_square += (diff * diff) / static_cast<double>(EXPECTED_FREQ);
        }

        // 当自由度为23时，置信水平为0.05，临界值35.17
        static constexpr double CRITICAL_VALUE = 35.17;
        return chi_square < CRITICAL_VALUE;
    }

    template <RandNumGenerator RNG> auto countFailures(const uz epochs) -> uz {
        uz num_failures = 0;
        for (uz i = 0; i < epochs; ++i) {
            if (not testShuffleQuality<RNG>()) {
                ++num_failures;
            }
        }
        return num_failures;
    }

    TEST_CASE("test shuffle quality") {

        static constexpr uz TEST_EPOCHS  = 1000;
        static constexpr uz MAX_FAILURES = 100;

        SUBCASE("test std::mt19937_64") {
            const uz num_fails = countFailures<std::mt19937_64>(TEST_EPOCHS);
            // fmt::println("mt19937_64: {} failures", num_fails);
            CHECK_LT(num_fails, MAX_FAILURES);
        }

        SUBCASE("test SplitMix64") {
            const uz num_fails = countFailures<SplitMix64>(TEST_EPOCHS);
            // fmt::println("SplitMix64: {} failures", num_fails);
            CHECK_LT(num_fails, MAX_FAILURES);
        }

        SUBCASE("test RomuTrio64") {
            const uz num_fails = countFailures<RomuTrio64>(TEST_EPOCHS);
            // fmt::println("RomuTrio64: {} failures", num_fails);
            CHECK_LT(num_fails, MAX_FAILURES);
        }

        SUBCASE("test WyRand64") {
            const uz num_fails = countFailures<WyRand64>(TEST_EPOCHS);
            // fmt::println("WyRand64: {} failures", num_fails);
            CHECK_LT(num_fails, MAX_FAILURES);
        }
    }
}

}
