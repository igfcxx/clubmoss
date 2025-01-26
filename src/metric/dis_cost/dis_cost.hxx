#ifndef CLUBMOSS_DIS_COST_HXX
#define CLUBMOSS_DIS_COST_HXX

#include "dis_cost_data.hxx"

namespace clubmoss::metric {

// 距离代价 //
class DisCost {
public:
    explicit DisCost(const dis_cost::Data &data);

    auto measure(const Layout&) -> fz;
    auto analyze(const Layout&) -> std::pair<fz, uz>;
    auto scan(const Layout&, Toml& stats) -> std::pair<fz, uz>;

    DisCost() = delete;

protected:
    fz cost_{0.0}; // 代价分数
    uz flaw_count_{0}; // 缺陷数

    std::array<fz, Finger::_size()> finger_move_{0.0}; // 手指移动距离

    std::array<fz, Finger::_size()> finger_usage_{0.0}; // 每个手指的使用率
    std::array<bool, Finger::_size()> is_finger_overused_{true}; // 手指是否过度使用

    fz left_hand_usage_{0.5}; // 左手使用率
    fz right_hand_usage_{0.5}; // 右手使用率
    bool is_hand_unbalanced_{true}; // 左右手使用是否不均衡

    auto calcMovementsOfEachFinger(const Layout& layout) noexcept -> void;

    auto calcFingerUsage() noexcept -> void;

private:
    inline static Config& cfg_ = Config::getInstance();

    dis_cost::Data data_;

    auto updateUsage(const Layout&, Cap prev_cap, Cap next_cap, fz freq) noexcept -> void;
    auto updateUsage(const Layout&, Cap cap, fz freq) noexcept -> void;

    auto validateUsage() noexcept -> void;
    auto collectStats() noexcept -> void;

    friend class clubmoss::Evaluator;
};

}

#endif // CLUBMOSS_DIS_COST_HXX
