#include <omp.h>
#include <doctest/doctest.h>

#include "../../src/module/prepropcessor/p_pool.hxx"
#include "../test_utilities.hxx"

namespace clubmoss::preprocessor::test {

TEST_SUITE("Test preprocessor::Pool") {

    class PoolWrapper : public Pool {
    public:
        auto getBestSample() const -> Sample& {
            return *samples_.front();
        }

        auto getBestLoss() const -> fz {
            return samples_.front()->getLoss();
        }

        auto getEps() const -> uz {
            return curr_epoch_;
        }

        auto getSep() const -> uz {
            return stagnation_epochs_;
        }
    };

    PoolWrapper pool;

    TEST_CASE("show extremes") {
        omp_set_num_threads(12);
        printTitle("Show extremes:");
        std::array<std::string, 6> metrics = {
            "[zh] key cost", "[en] key cost",
            "[zh] dis cost", "[en] dis cost",
            "[zh] seq cost", "[en] seq cost",
        };
        for (uz i = 0; i < 6; i++) {
            fmt::println(stderr, "{:s}", metrics[i]);
            const fz min = pool.search4min(i);
            const fz max = pool.search4max(i);
            fmt::println(
                stderr,
                "\nmin = {:.3f} {:4d} / {:4d} epochs"
                "\nmax = {:.3f} {:4d} / {:4d} epochs",
                min, pool.getSep(), pool.getEps(),
                max, pool.getSep(), pool.getEps()
            );
        }
        blankLine();
    }

    TEST_CASE("eee") {
        pool.search4min(0);
    }
}

}
