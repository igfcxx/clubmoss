#include <omp.h>
#include <doctest/doctest.h>

#include "../../src/module/optimizer/optimizer.hxx"

namespace clubmoss::optimizer::test {

TEST_CASE("test Optimizer::run()") {
    omp_set_num_threads(12);
    Optimizer opr;
    opr.search();
}

}
