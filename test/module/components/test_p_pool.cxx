#include <omp.h>
#include <doctest/doctest.h>

#include "../../../src/module/preprocessor/p_pool.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::preprocessor::test {

TEST_SUITE("Test preprocessor::Pool") {

    class PoolWrapper : public Pool {
    public:
        [[nodiscard]] auto getBestSample() const -> Sample& {
            return *samples_.front();
        }

        [[nodiscard]] auto getBestLoss() const -> fz {
            return samples_.front()->getLoss();
        }

        [[nodiscard]] auto getCurr() const -> uz {
            return curr_epoch_;
        }

        [[nodiscard]] auto getBest() const -> uz {
            return best_epoch_;
        }
    };

    PoolWrapper pool;

    TEST_CASE("show extremes") {
        omp_set_num_threads(12);
        std::array<std::string, TASK_COUNT> metrics = {
            "[zh] key cost", "[en] key cost",
            "[zh] dis cost", "[en] dis cost",
            "[zh] seq cost", "[en] seq cost",
        };
        for (uz task = 0; task < TASK_COUNT; task++) {
            fmt::println("\n{:s}", metrics[task]);
            const fz min = pool.search4min(task);
            const fz max = pool.search4max(task);
            fmt::println(
                "min = {:.3f} {:3d} / {:3d} epochs\n"
                "max = {:.3f} {:3d} / {:3d} epochs",
                min, pool.getBest(), pool.getCurr(),
                max, pool.getBest(), pool.getCurr()
            );
        }
    }

    TEST_CASE("try different pool sizes") {
        static constexpr std::array<uz, 5> SIZES{
            4800, 2400, 1200, 600, 300
        };
        omp_set_num_threads(12);
        spdlog::set_level(spdlog::level::debug);
        std::array<std::string, TASK_COUNT> metrics = {
            "[zh] key cost", "[en] key cost",
            "[zh] dis cost", "[en] dis cost",
            "[zh] seq cost", "[en] seq cost",
        };
        for (uz task = 0; task < TASK_COUNT; task++) {
            fmt::println("\n{:s}", metrics[task]);
            for (const uz size : SIZES) {
                pool.setSize(size);
                const fz min = pool.search4min(task);
                fmt::println(
                    "size = {:4d}, min = {:.3f}, {:3d} / {:3d} epochs",
                    size, min, pool.getBest(), pool.getCurr()
                );
            }
            for (const uz size : SIZES) {
                pool.setSize(size);
                const fz max = pool.search4max(task);
                fmt::println(
                    "size = {:4d}, max = {:.3f}, {:3d} / {:3d} epochs",
                    size, max, pool.getBest(), pool.getCurr()
                );
            }
        }
    }
}

}
