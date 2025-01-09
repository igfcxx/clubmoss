#ifndef CLUBMOSS_LAYOUT_CONFIG_HXX
#define CLUBMOSS_LAYOUT_CONFIG_HXX

#include "layout_area.hxx"

namespace clubmoss::layout {

// 布局设置 //
class Config final {
public:
    Config(Config&&)                 = delete;
    Config(const Config&)            = delete;
    Config& operator=(Config&&)      = delete;
    Config& operator=(const Config&) = delete;

    static auto getInstance() -> Config&;

    auto loadCfg(const Toml& cfg) -> void;

protected:
    std::vector<Area> mutable_areas_{}; // 可变区域列表
    std::vector<Key> pinned_keys_{};    // 固定按键列表
    std::vector<uz> area_ids_{};        // 区域编号列表

    uz num_mutable_keys_{}; // 可变按键数量
    uz num_pinned_keys_{};  // 固定按键数量
    uz num_areas_{};        // 可变区域数量

    Config();

    auto validateConfig(const Toml& cfg) -> void;
    auto loadPinnedKeys(const Toml& cfg) -> void;
    auto loadGivenAreas(const Toml& cfg) -> void;

    auto buildLastArea() -> void;

private:
    // 存储了按键对应的 Toml 对象的指针, 用于后续的验证和处理
    std::array<const Toml*, MAX_KEY_CODE> where_{nullptr};

    static constexpr uz MIN_MUTABLE_KEYS = 4;

    static auto validateFieldSize(const Toml& area_cfg) -> void;
    auto validateKeyValues(const Toml& area_cfg) -> void;
    auto validatePositions(const Toml& area_cfg) -> void;
    auto validateCap(const Toml& cap) -> void;
    auto validatePos(const Toml& pos) -> void;

    [[nodiscard]] auto observed(uz) const -> bool;

    auto resetCriticalMembers() -> void;

    class IllegalCfg final : public FatalError {
    public:
        IllegalCfg() = delete;

        explicit IllegalCfg(const std::string_view msg) noexcept
            : FatalError(std::format(WHAT, msg)) {}

        template <typename TC, typename... Ts>
        explicit IllegalCfg(
            const std::string& title,
            const toml::basic_value<TC>& v,
            const std::string& msg, Ts&&... tail
        ) noexcept
            : FatalError(std::format(WHAT, format_error(title, v, msg, std::forward<Ts>(tail)...))) {}

    private:
        static constexpr auto WHAT{
            "Illegal layout configuration: {:s}"
        };
    };

    friend class Manager;

    inline static const auto DEFAULT_CFG = u8R"(
        [[mutable_areas]]
        cap_list = ["Z", "X", "C", "V"]
        pos_list = [20, 21, 22, 23]

        [[pinned_keys]]
        cap = ","
        pos = 27

        [[pinned_keys]]
        cap = "."
        pos = 28

        [[pinned_keys]]
        cap = "/"
        pos = 29

        [[pinned_keys]]
        cap = ";"
        pos = 9
    )"_toml;
};

}

#endif //CLUBMOSS_LAYOUT_CONFIG_HXX
