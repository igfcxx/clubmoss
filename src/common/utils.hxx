#ifndef CLUBMOSS_UTILS_HXX
#define CLUBMOSS_UTILS_HXX

#include <array>
#include <vector>
#include <string>
#include <format>
#include <ranges>
#include <memory>
#include <filesystem>

#include <toml.hpp>

#include "types.hxx"

namespace clubmoss {

using Toml = toml::value;

static constexpr uz ROW_COUNT = 3;  // 行数
static constexpr uz COL_COUNT = 10; // 列数
static constexpr uz KEY_COUNT = 30; // 按键数

static constexpr uz KEY_CNT_POW2 = 32; // 30 -> 32
static constexpr uz MAX_KEY_CODE = 92; // 90 -> 92

static constexpr std::array<Cap, KEY_COUNT> CAP_SET{
    ',', '.', '/', ';', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
};

static constexpr std::array<Pos, KEY_COUNT> POS_SET = [] -> auto {
    std::array<Pos, KEY_COUNT>&& temp{};
    for (uz i = 0; i < KEY_COUNT; ++i) {
        temp[i] = static_cast<Pos>(i);
    }
    return temp;
}();

class Utils final {
public:
    static auto absPath(std::string_view sub_path) -> std::string;

    template <typename INT> requires std::is_integral_v<INT>
    static auto isLegalCap(const INT cap) noexcept -> bool {
        return (cap >= 'A' and cap <= 'Z') or cap == ',' or
                cap == '.' or cap == ';' or cap == '/';
    }

    template <typename INT> requires std::is_integral_v<INT>
    static auto isLegalPos(const INT pos) noexcept -> bool {
        return 0 <= pos and pos < KEY_COUNT;
    }

    template <typename INT> requires std::is_integral_v<INT>
    static auto isLegalCol(const INT col) noexcept -> bool {
        return 0 <= col and col < COL_COUNT;
    }

    template <typename INT> requires std::is_integral_v<INT>
    static auto isLegalRow(const INT row) noexcept -> bool {
        return 0 <= row and row < ROW_COUNT;
    }

    static auto posFromRowCol(Row, Col) noexcept -> Pos;
    static auto colOfPos(Pos) noexcept -> Col;
    static auto rowOfPos(Pos) noexcept -> Row;

private:
    using FileNames     = std::vector<std::string>;
    using RequiredFiles = std::pair<std::string, FileNames>;
    inline static const std::vector<RequiredFiles> REQUIRED_FILES{
        {"conf", {"layout.toml"}},
    };

    static auto searchForProjectRoot() -> std::string;
    static auto findAllRequiredFiles(const std::filesystem::path& path) -> bool;
};

}

#endif //CLUBMOSS_UTILS_HXX
