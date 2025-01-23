/* =============================================================================

                  _____  _       _
                 |     || | _ _ | |_  _____  ___  ___  ___
                 |   --|| || | || . ||     || . ||_ -||_ -|
                 |_____||_||___||___||_|_|_||___||___||___|


本项目仅考察标准 ANSI/ISO 键盘的中心区域, 即 QWERTY 键盘中由以下按键所组成的 3×10 区域的布局:

                            Q W E R T Y U I O P
                            A S D F G H J K L ;
                            Z X C V B N M , . /

==============================================================================*/


#ifndef CLUBMOSS_TYPES_HXX
#define CLUBMOSS_TYPES_HXX

#include <cfloat>
#include <random>

#include <better-enums/enum.h>
#include <toml.hpp>

#include "prngs.hxx"

#define TOML11_NO_ERROR_PREFIX

namespace clubmoss {

using u8 = uint8_t; // 最小无符号整型
using uz = size_t; // 原生无符号整型
using fz = double; // 默认浮点型

using Cap = u8; // 键值(ASCII), 所有大写字母 外加 ,.;/ 四个符号
using Pos = u8; // 键位(编号), ∈ [0, 29]
using Col = u8; // 列号, ∈ [0, 9]
using Row = u8; // 行号, ∈ [0, 2]

struct Key final {
    Cap cap{0};
    Pos pos{0};

    Key() = delete;

    Key(const Cap cap, const Pos pos)
        : cap(cap), pos(pos) {}
};

using Prng = prng::RomuTrio64;

// @formatter:off //
BETTER_ENUM(
    Language, uz,
    Chinese = 0,
    English = 1
)

BETTER_ENUM(
    MetricId, uz,
    KeyCost = 0,
    DisCost = 1,
    SeqCost = 2
)

BETTER_ENUM(
    Finger, uz,
    LeftPinky   = 0,
    LeftRing    = 1,
    LeftMiddle  = 2,
    LeftIndex   = 3,
    LeftThumb   = 4,
    RightThumb  = 5,
    RightIndex  = 6,
    RightMiddle = 7,
    RightRing   = 8,
    RightPinky  = 9
)

BETTER_ENUM(
    PosRelation, uz,
    SamePosition = 0,
    SameFinger   = 1,
    SameHand     = 2,
    None         = 3
)

BETTER_ENUM(
    PainLevel, uz,
    No       = 0,
    Mild     = 1,
    Moderate = 2,
    Severe   = 3,
    Extreme  = 4
)
// @formatter:on //

class FatalError : public std::runtime_error {
public:
    FatalError() = delete;

    explicit FatalError(const std::string_view msg) noexcept
        : std::runtime_error(std::format("[error] {}", msg)) {}
};

template <const char* WHAT>
class IllegalToml final : public FatalError {
public:
    IllegalToml() = delete;

    explicit IllegalToml(const std::string_view msg) noexcept
        : FatalError(std::format(WHAT, msg)) {}

    template <typename TC, typename... Ts>
    explicit IllegalToml(
        const std::string& title,
        const toml::basic_value<TC>& v,
        const std::string& msg, Ts&&... tail
    ) noexcept
        : FatalError(std::format(WHAT, format_error(title, v, msg, std::forward<Ts>(tail)...))) {}
};

}

#endif //CLUBMOSS_TYPES_HXX
