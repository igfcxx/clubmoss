#include <omp.h>
#include <doctest/doctest.h>

#include "../../../src/module/optimizer/o_pool.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::optimizer::test {

TEST_SUITE("Test optimizer::Pool") {

    class PoolWrapper : public Pool {
    public:
        auto getBestSample() const -> Sample& {
            return *samples_.front();
        }

        auto getBestLoss() const -> fz {
            return samples_.front()->getLoss();
        }
    };

    PoolWrapper pool;

    TEST_CASE("test optimizer::Pool::search()") {
        omp_set_num_threads(1);
        pool.setSize(1200);

        pool.search();
        const std::string s1 = pool.getBestSample().toString();
        const fz l1 = pool.getBestLoss();

        pool.search();
        const std::string s2 = pool.getBestSample().toString();
        const fz l2 = pool.getBestLoss();

        WARN_NE(s1, s2);
        WARN_NE(l1, l2);
    }

    TEST_CASE("show best sample in pools") {
        printTitle("Show best sample in pools:");
        for (uz i = 1; i <= 5; i++) {
            pool.search();
            fmt::println(
                stderr, "{:d}. {:s} - {:.3f}",
                i, pool.getBestSample().toString(), pool.getBestLoss()
            );
        }
        blankLine();
    }
}

}
