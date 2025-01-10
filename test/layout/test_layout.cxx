#include <doctest/doctest.h>

#include "../../src/layout/layout.hxx"
#include "../test_utilities.hxx"

namespace clubmoss::test {

TEST_SUITE("Test Layout") {

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
            REQUIRE_NOTHROW((Layout(QWERTY_SEQ)));
            REQUIRE_NOTHROW((Layout(DVORAK_SEQ)));
        }

        SUBCASE("fail") {
            printTitle("Show Layout(seq) error messages:");
            check([] { (Layout(LEN31_SEQ)); });
            check([] { (Layout(LOWER_SEQ)); });
            check([] { (Layout(COLON_SEQ)); });
            check([] { (Layout(DUP_Q_SEQ)); });
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
            // REQUIRE_FALSE(UnsafeLayout(LOWER_SEQ).isValid()); // debug 模式下无法通过断言
            // REQUIRE_FALSE(UnsafeLayout(COLON_SEQ).isValid()); // debug 模式下无法通过断言
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
