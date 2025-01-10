#include <doctest/doctest.h>

#include "../../src/layout/layout_manager.hxx"
#include "../test_utilities.hxx"

namespace clubmoss::layout::tests {

TEST_SUITE("Test layout::Manager") {

    TEST_CASE("test layout::Manager() construction") {

        SUBCASE("default config") {
            REQUIRE_NOTHROW(Manager());
        }

        SUBCASE("TOML file") {
            const std::string PATH = Utils::absPath("conf/layout.toml");
            Manager::loadCfg(toml::parse(PATH));
            REQUIRE_NOTHROW((Manager()));
        }
    }

    TEST_CASE("test copy construction and assignment") {

        SUBCASE("copy construction") {
            Manager mgr1, mgr2(mgr1);
            const Layout lyt1 = mgr1.create();
            const Layout lyt2 = mgr2.create();
            REQUIRE(mgr1.canManage(lyt2));
            REQUIRE(mgr2.canManage(lyt1));
            REQUIRE_NE(lyt1, lyt2);
        }

        SUBCASE("copy assignment") {
            Manager mgr1, mgr2 = mgr1;
            const Layout lyt1  = mgr1.create();
            const Layout lyt2  = mgr2.create();
            REQUIRE(mgr1.canManage(lyt2));
            REQUIRE(mgr2.canManage(lyt1));
            REQUIRE_NE(lyt1, lyt2);
        }
    }

    static Manager manager;

    const Layout EXAMPLE = manager.create();

    /**
     * @brief 检验创建的布局的随机性.
     * @param func 待测函数, 返回 Layout.
     * @param samples 测试样本的数量.
     * @param threshold 最大重复数.
     */
    template <typename Func>
        requires std::is_invocable_r_v<Layout, Func>
    auto checkRandomness(Func func, const uz samples, const uz threshold) -> void {
        std::vector<Layout> layouts;
        for (uz i = 0; i < samples; i++) {
            layouts.emplace_back(func());
        }

        std::ranges::sort(layouts);
        uz num_duplicate_items = 0;
        for (uz i = 0; i < layouts.size() - 1; ++i) {
            if (layouts[i] == layouts[i + 1]) {
                ++num_duplicate_items;
            }
        }
        CHECK_LT(num_duplicate_items, threshold);
    }

    TEST_CASE("test layout::Manager::create()") {
        auto wrapper = [&]() -> Layout { return manager.create(); };
        checkRandomness(wrapper, 1000, 10);
    }

    TEST_CASE("show layout::Manager::create()") {
        printTitle("Show layout::Manager::create() examples:");
        for (uz i = 1; i <= 5; i++) {
            Layout layout = manager.create();
            std::string s = layout.toCapSeq();
            fmt::println(stderr, "{:d}. {:s}", i, s);
        }
        blankLine();
    }

    TEST_CASE("test layout::Manager::reinit()") {

        SUBCASE("compare with previous") {
            Layout layout = EXAMPLE;
            manager.reinit(layout);
            REQUIRE_GT(munOfDiffKeys(EXAMPLE, layout), 0);
        }

        SUBCASE("verify randomness") {
            auto wrapper = [&]() -> Layout {
                Layout l = EXAMPLE;
                manager.reinit(l);
                return l;
            };
            checkRandomness(wrapper, 1000, 10);
        }
    }

    TEST_CASE("test layout::Manager::mutate()") {

        SUBCASE("compare with parent") {
            Layout layout = EXAMPLE;
            manager.mutate(layout, EXAMPLE);
            REQUIRE_EQ(munOfDiffKeys(layout, EXAMPLE), 2);
        }

        SUBCASE("verify randomness") {
            auto wrapper = [&]() -> Layout {
                Layout layout = EXAMPLE;
                manager.mutate(layout, EXAMPLE);
                return layout;
            };
            checkRandomness(wrapper, 100, 30);
        }
    }

    TEST_CASE("show layout::Manager::mutate()") {
        printTitle("Show layout::Manager::mutate() examples:");
        Layout child = EXAMPLE;
        for (uz i = 0; i < 5; i++) {
            Layout parent = manager.create();
            manager.mutate(child, parent);

            std::vector<uz> idx{};
            for (const Pos pos : POS_SET) {
                if (child.getCap(pos) != parent.getCap(pos)) {
                    idx.emplace_back(pos);
                }
            }
            REQUIRE_EQ(idx.size(), 2);

            fmt::print(
                stderr,
                "{:d}.\n"
                "parent: {:s}\n"
                "child:  {:s}\n"
                "diff:   {:s}^{:s}^\n\n",
                i + 1, parent.toCapSeq(), child.toCapSeq(),
                std::string(idx[0], ' '), std::string(idx[1] - idx[0] - 1, ' ')
            );
        }
    }

    // TEST_CASE("test reinit() and mutate() assertions") {
    //     // Dvorak 键盘布局无法在默认配置下管理.
    //     Layout unmanageable(baselines::DVORAK);
    //     CHECK_FALSE(manager.canManage(unmanageable));
    //
    //     SUBCASE("reinit()") {
    //         // 在调试模式下, 会引发断言异常;
    //         // 在发布模式下, 产生未定义行为.
    //         manager.reinit(unmanageable);
    //     }
    //
    //     SUBCASE("mutate()") {
    //         // 在调试模式下, 会引发断言异常;
    //         // 在发布模式下, 产生未定义行为.
    //         Layout layout = manager.create();
    //         manager.mutate(layout, unmanageable);
    //     }
    // }
}

}
