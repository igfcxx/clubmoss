#include <doctest/doctest.h>

#include "../../../src/metric/key_cost/key_cost_data.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::metric::key_cost::test {

TEST_SUITE("Test metric::key_cost::Data") {

    static auto findTestFiles(const std::string& sub_dir) -> PathVec {
        static const std::string root = Utils::absPath("test/metric/key_cost/data_to_test");
        return collectTestFiles(root, {sub_dir});
    }

    TEST_CASE("test metric::key_cost::Data(Toml) construction") {

        SUBCASE("should pass") {
            static const PathVec test_files = findTestFiles("should_pass");
            for (const fs::path& file : test_files) {
                const Toml& t = toml::parse(file);
                REQUIRE_NOTHROW((Data(t)));
            }
        }

        SUBCASE("should fail") {
            printTitle("Show metric::key_cost::Config::loadCfg(Toml) error messages:");
            static const PathVec test_files = findTestFiles("should_fail");
            for (const fs::path& file : test_files) {
                const Toml& t = toml::parse(file);
                check([&] { (Data(t)); });
            }
        }
    }

}

}
