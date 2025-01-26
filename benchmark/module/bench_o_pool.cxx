#include <omp.h>
#include <thread>

#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/module/optimizer/o_pool.hxx"

static constexpr size_t MAX_THREADS = 12;

namespace clubmoss::optimizer::bench {

// 可以暂时将 max_stagnation_epochs_ 和 MAX_EPOCHS 设置为相同的值,
// 并注释掉updateMse(), 以控制收敛速度, 获得更稳定的结果.

TEST_SUITE("Bench optimizer::Pool::search()") {

    TEST_CASE("bench optimizer::Pool::search()") {
        REQUIRE_LE(MAX_THREADS, std::thread::hardware_concurrency());
        REQUIRE_GE(MAX_THREADS, 1);

        Pool pool;

        ankerl::nanobench::Bench b;

        b.title("Pool::search()")
         .unit("Pool.epoch")
         .relative(true)
         .epochs(3);
        b.performanceCounters(true);

        omp_set_num_threads(1);
        b.run("baseline", [&]() -> void { pool.search(); });
        for (int i = 2; i <= MAX_THREADS; i += 2) {
            omp_set_num_threads(i);
            b.run(
                fmt::format("{:d} threads", i).c_str(),
                [&]() -> void { pool.search(); }
            );
        }
    }
}

}
