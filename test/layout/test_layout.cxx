#include <doctest/doctest.h>

#include "../../src/layout/layout.hxx"
#include "../utilities.hxx"

namespace clubmoss::test {

TEST_SUITE("Test Layout") {

    #define CLUBMOSS_SHOW_ERR_MSG

    auto check(const std::string_view seq) -> void {
        try {
            Layout l(seq);
            REQUIRE(0); // 不应执行
        } catch (const std::exception& e) {
            fmt::println("\n{}", e.what());
        } catch (...) {
            fmt::println("\nUnknown exception");
            REQUIRE(false); // 不应执行
        }
    }

    // 合法序列
    static const std::string QWERTY_SEQ = "QWERTYUIOPASDFGHJKL;ZXCVBNM,./";
    static const std::string DVORAK_SEQ = "/,.PYFGCRLAOEUIDHTNS;QJKXBMWVZ";

    // 非法序列
    static const std::string LEN31_SEQ = "QWERTYUIOPASDFGHJKL;ZXCVBNM,.//"; // 非法序列长度 31
    static const std::string LOWER_SEQ = "qwertyuiopasdfghjkl;zxcvbnm,./";  // 非法字符 q
    static const std::string COLON_SEQ = "QWERTYUIOPASDFGHJKL:ZXCVBNM,./";  // 非法字符 :
    static const std::string DUP_Q_SEQ = "QQQRTYUIOPASDFGHJKL;ZXCVBNM,./";  // 重复键值 Q

    TEST_CASE("test Layout(seq) construction") {

        SUBCASE("success") {
            REQUIRE_NOTHROW(Layout _(QWERTY_SEQ));
            REQUIRE_NOTHROW(Layout _(DVORAK_SEQ));
        }

        SUBCASE("fail") {
            // 定义 CLUBMOSS_SHOW_ERR_MSG 宏以查看报错信息.
            #ifdef CLUBMOSS_SHOW_ERR_MSG
            fmt::println("\nShow Layout(seq) error message:");
            check(LEN31_SEQ); check(LOWER_SEQ);
            check(COLON_SEQ); check(DUP_Q_SEQ);
            fmt::print("\n");
            #else
            REQUIRE_THROWS_AS((Layout(LEN31_SEQ)), FatalError);
            REQUIRE_THROWS_AS((Layout(LOWER_SEQ)), FatalError);
            REQUIRE_THROWS_AS((Layout(COLON_SEQ)), FatalError);
            REQUIRE_THROWS_AS((Layout(DUP_Q_SEQ)), FatalError);
            #endif
        }
    }

    TEST_CASE("test Layout verification") {

        class UnsafeLayout : public Layout {
        public:
            explicit UnsafeLayout(const std::string_view seq) {
                loadFromSeq(seq); // 不对 seq 进行合法性检查
            }
        };

        SUBCASE("valid") {
            REQUIRE(UnsafeLayout(QWERTY_SEQ).isValid());
            REQUIRE(UnsafeLayout(DVORAK_SEQ).isValid());
        }

        SUBCASE("invalid") {
            // REQUIRE_FALSE(UnsafeLayout(LOWER_SEQ).isValid()); // 产生断言失败
            // REQUIRE_FALSE(UnsafeLayout(COLON_SEQ).isValid()); // 产生断言失败
            REQUIRE_FALSE(UnsafeLayout(DUP_Q_SEQ).isValid());
        }
    }

    TEST_CASE("test Layout comparison") {

        SUBCASE("operator==") {
            Layout l1("QWERTYUIOPASDFGHJKL;ZXCVBNM,./"); // QW...
            Layout l2("QWERTYUIOPASDFGHJKL;ZXCVBNM,./"); // QW...
            Layout l3("WQERTYUIOPASDFGHJKL;ZXCVBNM,./"); // WQ...
            REQUIRE((l1 == l2 and l1 != l3));
        }

        SUBCASE("sort") {
            Layout la("ABCDEFGHIJKLMNOPQRSTUVWXYZ,.;/"); // AB...
            Layout lb("BCDEFGHIJKLMNOPQRSTUVWXYZ,.;/A"); // BC...
            Layout lc("CDEFGHIJKLMNOPQRSTUVWXYZ,.;/AB"); // CD...
            std::vector layouts{lc, lb, la};
            std::ranges::sort(layouts);
            CHECK_EQ(layouts[0], la);
            CHECK_EQ(layouts[1], lb);
            CHECK_EQ(layouts[2], lc);
        }
    }
}

}
