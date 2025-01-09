#ifndef TEST_UTILS_HXX
#define TEST_UTILS_HXX

#include <functional>

#include <fmt/core.h>

#include "../src/layout/layout.hxx"

namespace clubmoss {

#define CLUBMOSS_SHOW_ERR_MSG

namespace fs = std::filesystem;

static auto printTitle(const std::string& title) -> void {
    #ifdef CLUBMOSS_SHOW_ERR_MSG
    const uz len = title.length();
    fmt::println(stderr,"\n{}", title);
    fmt::println(stderr,"{}", std::string(len, '-'));
    #endif
}

static auto blankLine() -> void {
    #ifdef CLUBMOSS_SHOW_ERR_MSG
    fmt::print(stderr,"\n");
    #endif
}

template <typename Func>
    requires std::is_invocable_v<Func>
auto check(Func&& func) -> void {
    #ifdef CLUBMOSS_SHOW_ERR_MSG
    try {
        std::forward<Func>(func)();
        REQUIRE(false); // no throw
    } catch (const std::exception& e) {
        fmt::print(stderr,"{}\n\n", e.what());
    } catch (...) { // unknown exception
        REQUIRE(false);
    }
    #else
    REQUIRE_THROWS_AS(std::forward<Func>(func)(), std::exception);
    #endif
}

using PathVec = std::vector<fs::path>;

static auto collectTomlFiles(
    const fs::path& dir,
    const uz curr_depth,
    PathVec& paths
) -> void {
    static constexpr uz MAX_DEPTH = 3;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (curr_depth <= MAX_DEPTH and entry.is_directory()) {
            collectTomlFiles(entry.path(), curr_depth + 1, paths);
        } else if (entry.is_regular_file() and entry.path().extension() == ".toml") {
            paths.emplace_back(entry.path().generic_string());
        }
    }
}

static auto collectTestFiles(
    const std::string& root,
    const std::initializer_list<std::string>& sub_dirs
) -> PathVec {
    PathVec test_files;
    for (const std::string& sub_dir : sub_dirs) {
        collectTomlFiles(fs::path(root) / sub_dir, 0, test_files);
    }
    return test_files;
}


namespace layout {
    static auto munOfDiffKeys(const Layout& lyt1, const Layout& lyt2) -> uz {
        uz counter = 0;
        for (const auto &keys1 = lyt1.toCapSeq(), &keys2 = lyt2.toCapSeq();
             auto [key1, key2] : std::views::zip(keys1, keys2)) {
            if (key1 != key2) {
                ++counter;
            }
        }
        return counter;
    }
}

}

#endif //TEST_UTILS_HXX
