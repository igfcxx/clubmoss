#ifndef CLUBMOSS_DEFAULT_LAYOUT_CONFIGS_HXX
#define CLUBMOSS_DEFAULT_LAYOUT_CONFIGS_HXX

#include "../../src/common/utils.hxx"

namespace clubmoss::layout::bench {

static const Toml CFG_1_AREA = u8R"(
    [[pinned_keys]]
    cap = ";"
    pos = 9

    [[pinned_keys]]
    cap = ","
    pos = 27

    [[pinned_keys]]
    cap = "."
    pos = 28

    [[pinned_keys]]
    cap = "/"
    pos = 29
)"_toml;

static const Toml CFG_2_AREA = u8R"(
    [[mutable_areas]]
    cap_list = ["Z", "X", "C", "V"]
    pos_list = [20, 21, 22, 23]

    [[pinned_keys]]
    cap = ";"
    pos = 9

    [[pinned_keys]]
    cap = ","
    pos = 27

    [[pinned_keys]]
    cap = "."
    pos = 28

    [[pinned_keys]]
    cap = "/"
    pos = 29
)"_toml;

static const Toml CFG_3_AREA = u8R"(
    [[mutable_areas]]
    cap_list = ["A", "E", "I", "O", "U", "N", "H", "T"]
    pos_list = [10, 11, 12, 13, 16, 17, 18, 19]

    [[mutable_areas]]
    cap_list = ["Z", "X", "C", "V"]
    pos_list = [20, 21, 22, 23]

    [[pinned_keys]]
    cap = ";"
    pos = 9

    [[pinned_keys]]
    cap = ","
    pos = 27

    [[pinned_keys]]
    cap = "."
    pos = 28

    [[pinned_keys]]
    cap = "/"
    pos = 29
)"_toml;

}

#endif //CLUBMOSS_DEFAULT_LAYOUT_CONFIGS_HXX
