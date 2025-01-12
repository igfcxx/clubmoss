#include <doctest/doctest.h>

#include "../../../src/metric/key_cost/key_cost_config.hxx"
#include "../../test_utilities.hxx"

namespace clubmoss::metric::key_cost::test {

TEST_SUITE("Test metric::key_cost::Config") {

    Config& cfg = Config::getInstance();

    static auto findTestFiles(const std::string& sub_dir) -> PathVec {
        static const std::string root = Utils::absPath("test/metric/key_cost/cfgs_to_test");
        return collectTestFiles(root, {sub_dir});
    }

    TEST_CASE("test metric::key_cost::Config::loadCfg(Toml)") {

        SUBCASE("should pass") {
            static const PathVec test_files = findTestFiles("should_pass");
            for (const fs::path& file : test_files) {
                const Toml& t = toml::parse(file);
                REQUIRE(t.contains("key_cost"));
                REQUIRE_NOTHROW(cfg.loadCfg(t.at("key_cost")));
            }
        }

        SUBCASE("should fail") {
            printTitle("Show metric::key_cost::Config::loadCfg(Toml) error messages:");
            static const PathVec test_files = findTestFiles("should_fail");
            for (const fs::path& file : test_files) {
                const Toml& t = toml::parse(file);
                REQUIRE(t.contains("key_cost"));
                check([&] { cfg.loadCfg(t.at("key_cost")); });
            }
        }
    }

}

}
