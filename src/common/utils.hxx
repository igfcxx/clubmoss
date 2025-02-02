#ifndef CLUBMOSS_UTILS_HXX
#define CLUBMOSS_UTILS_HXX

#include <map>
#include <set>
#include <array>
#include <vector>
#include <bitset>
#include <string>
#include <format>
#include <ranges>
#include <memory>
#include <utility>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

#include "types.hxx"

namespace clubmoss {

using Toml = toml::basic_value<toml::ordered_type_config>;

static constexpr uz ROW_COUNT = 3; // 行数
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

static constexpr uz TASK_COUNT = MetricId::_size() * Language::_size();

using namespace toml::literals::toml_literals;

class Utils final {
public:
    static auto absPath(std::string_view sub_path) -> std::string;

    static auto colOf(Pos) noexcept -> Col;
    static auto rowOf(Pos) noexcept -> Row;
    static auto posOf(Row, Col) noexcept -> Pos;

    static auto taskIdOf(MetricId, Language) noexcept -> uz;

    static auto toSnakeCase(std::string_view pascal_case) -> std::string;

    template <typename INT> requires std::is_integral_v<INT>
    static auto isLegalCap(const INT cap) noexcept -> bool {
        return (cap >= 'A' and cap <= 'Z') or
                cap == ',' or cap == '.' or
                cap == ';' or cap == '/';
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

    template <typename Container>
    static auto sum(const Container& container) -> typename Container::value_type {
        return std::accumulate(container.begin(), container.end(), static_cast<typename Container::value_type>(0.0));
    }

private:
    using FileNames = std::vector<std::string>;
    using RequiredFiles = std::pair<std::string, FileNames>;
    inline static const std::vector<RequiredFiles> REQUIRED_FILES{
        {"conf", {"layout.toml", "metric.toml", "score.toml"}},
        {"data/chinese", {"char.toml", "pair.toml", "seq.toml"}},
        {"data/english", {"char.toml", "pair.toml", "seq.toml"}},
        {"cache", {"status.toml"}},
    };

    static auto searchForProjectRoot() -> std::string;
    static auto findAllRequiredFiles(const std::filesystem::path& path) -> bool;
};

class LogSink final
        : public spdlog::sinks::base_sink<std::mutex> {
public:
    using LogCallback = std::function<void(const std::string&)>;

    explicit LogSink(LogCallback callback) : callback_(std::move(callback)) {}

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        formatter_->format(msg, formatted);
        const std::string log_message = fmt::to_string(formatted);
        if (callback_) { callback_(log_message); }
    }

    void flush_() override {}

private:
    LogCallback callback_;
};

}

#endif //CLUBMOSS_UTILS_HXX
