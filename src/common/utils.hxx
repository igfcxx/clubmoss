#ifndef CLUBMOSS_UTILS_HXX
#define CLUBMOSS_UTILS_HXX

#include <map>
#include <array>
#include <vector>
#include <bitset>
#include <string>
#include <format>
#include <ranges>
#include <memory>
#include <utility>
#include <filesystem>

#include "types.hxx"

namespace clubmoss {

using Toml = toml::basic_value<toml::ordered_type_config>;

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

using namespace toml::literals::toml_literals;

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

    static auto posOf(Row, Col) noexcept -> Pos;
    static auto colOf(Pos) noexcept -> Col;
    static auto rowOf(Pos) noexcept -> Row;

    template <typename Container>
    static auto sum(const Container& container) -> typename Container::value_type {
        return std::accumulate(container.begin(), container.end(), static_cast<typename Container::value_type>(0.0));
    }

    static auto toSnakeCase(std::string_view pascal_case) -> std::string;

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
