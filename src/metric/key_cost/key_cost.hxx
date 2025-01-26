#ifndef CLUBMOSS_KEY_COST_HXX
#define CLUBMOSS_KEY_COST_HXX

#include "key_cost_data.hxx"

namespace clubmoss::metric {

// 击键代价 //
class KeyCost {
public:
    explicit KeyCost(const key_cost::Data& data);

    auto measure(const Layout&) -> fz;
    auto analyze(const Layout&) -> std::pair<fz, uz>;
    auto scan(const Layout&, Toml& stats) -> std::pair<fz, uz>;

    KeyCost() = delete;

protected:
    fz cost_{0.0}; // 代价
    uz flaw_count_{0}; // 缺陷

    std::array<fz, KEY_COUNT> heat_map_{0.0}; // 热力图
    std::array<fz, ROW_COUNT> row_usage_{0.0}; // 行使用率
    std::array<fz, COL_COUNT> col_usage_{0.0}; // 列使用率

    std::array<fz, Finger::_size()> finger_usage_{0.0}; // 每个手指的使用率
    std::array<bool, Finger::_size()> is_finger_overused_{true}; // 手指是否过度使用

    fz left_hand_usage_{0.5}; // 左手使用率
    fz right_hand_usage_{0.5}; // 右手使用率
    bool is_hand_unbalanced_{true}; // 左右手使用是否不均衡

    fz similarity_{0.0}; // 与标准布局的相似度

    auto calcFingerUsage(const Layout&) noexcept -> void;

private:
    inline static Config& cfg_ = Config::getInstance();

    key_cost::Data data_;

    auto validateLayout() noexcept -> void;
    auto collectStats() noexcept -> void;

    friend class clubmoss::Evaluator;
};

}

#endif // CLUBMOSS_KEY_COST_HXX
