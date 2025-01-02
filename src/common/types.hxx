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

#include <random>

namespace clubmoss {

using u8 = uint8_t; // 最小无符号整型
using uz = size_t;  // 原生无符号整型
using fz = double;  // 默认浮点型

using Cap = u8; // 键值(ASCII), 所有大写字母 外加 ,.;/ 四个符号
using Pos = u8; // 键位(编号), ∈ [0, 29]
using Col = u8; // 列号, ∈ [0, 9]
using Row = u8; // 行号, ∈ [0, 2]

class SplitMix64 final {
public:
    using result_type = uint64_t;

    SplitMix64() : SplitMix64(42ull) {}

    explicit SplitMix64(const uint64_t seed) {
        this->seed(seed), warmup();
    }

    auto operator()() -> result_type {
        state_ += 0x9E3779B97F4A7C15ull; uint64_t z = state_;
        z = (z ^ (z >> 30ull)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27ull)) * 0x94D049BB133111EBull;
        return z ^ (z >> 31ull);
    }

    auto seed(const uint64_t seed) -> void {
        this->state_ = seed;
    }

    auto warmup() -> void {
        for (size_t i = 0; i < 5; ++i) {
            operator()();
        }
    }

    [[maybe_unused]] static constexpr auto min() -> result_type {
        return std::numeric_limits<result_type>::min();
    }

    [[maybe_unused]] static constexpr auto max() -> result_type {
        return std::numeric_limits<result_type>::max();
    }

private:
    uint64_t state_{};
};

using Prng = SplitMix64;

class FatalError : public std::runtime_error {
public:
    FatalError() = delete;

    explicit FatalError(const std::string_view msg) noexcept
        : std::runtime_error(std::format("Fatal Error: {}", msg)) {}
};

}

#endif //CLUBMOSS_TYPES_HXX
