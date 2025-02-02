#include <omp.h>
#include <doctest/doctest.h>

#include "../../src/module/preprocessor/preprocessor.hxx"

namespace clubmoss::preprocessor::test {

Preprocessor ppr;

TEST_CASE("test Preprocessor::searchExtremes()") {
    // spdlog::set_level(spdlog::level::debug);
    omp_set_num_threads(12);
    ppr.searchExtremes();
}

TEST_CASE("test Preprocessor::estimateSize()") {
    // spdlog::set_level(spdlog::level::debug);
    omp_set_num_threads(12);
    ppr.estimateSize();
}

TEST_CASE("test Preprocessor::run()") {
    omp_set_num_threads(12);
    ppr.run();
}

}
