#include <doctest/doctest.h>

#include "../../src/layout/layout_config.hxx"
#include "../test_utilities.hxx"

namespace clubmoss::layout::test {
TEST_SUITE("Test layout::Config") {

    Config& cfg = Config::getInstance();

    static auto findTestFiles(const std::string& sub_dir) -> PathVec {
        static const std::string root = Utils::absPath("test/layout/test_files");
        return collectTestFiles(root, {sub_dir});
    }

    TEST_CASE("test layout::Config::loadCfg(Toml)") {

        SUBCASE("should pass") {
            static const PathVec test_files = findTestFiles("should_pass");
            for (const fs::path& file : test_files) {
                const Toml& t = toml::parse(file);
                REQUIRE_NOTHROW(cfg.loadCfg(t));
            }
        }

        SUBCASE("should fail") {
            printTitle("Show layout::Config::loadCfg(Toml) error messages:");
            static const PathVec test_files = findTestFiles("should_fail");
            for (const fs::path& file : test_files) {
                const Toml& t = toml::parse(file);
                check([&] { cfg.loadCfg(t); });
            }
        }
    }
}

}
