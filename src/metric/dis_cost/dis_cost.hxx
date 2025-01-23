#ifndef CLUBMOSS_DIS_COST_HXX
#define CLUBMOSS_DIS_COST_HXX

#include "dis_cost_config.hxx"

namespace clubmoss::metric {

// 距离代价指标 //
class DisCost {
public:
    auto analyze(const Layout&, const dis_cost::Data& data) -> void;
    auto measure(const Layout&, const dis_cost::Data& data) -> fz;
    auto check(const Layout&, const dis_cost::Data& data) -> bool;

    static auto loadCfg(const Toml& cfg) -> void;

protected:
    fz cost_{0.0}; // 代价分数
    bool valid_{false}; // 是否有效

    std::array<fz, Finger::_size()> finger_move_{0.0}; // 手指移动距离

    std::array<fz, Finger::_size()> finger_usage_{0.0}; // 每个手指的使用率
    std::array<bool, Finger::_size()> is_finger_overused_{true}; // 手指是否过度使用

    fz left_hand_usage_{0.5}; // 左手使用率
    fz right_hand_usage_{0.5}; // 右手使用率
    bool is_hand_unbalanced_{true}; // 左右手使用是否不均衡

    auto calcMovementsOfEachFinger(const Layout& layout, const dis_cost::Data& data) noexcept -> void;

    auto calcFingerUsage() noexcept -> void;

private:
    inline static dis_cost::Config& cfg_ = dis_cost::Config::getInstance();

    auto updateUsage(const Layout&, Cap prev_cap, Cap next_cap, fz freq) noexcept -> void;
    auto updateUsage(const Layout&, Cap cap, fz freq) noexcept -> void;

    auto validateUsage() noexcept -> void;
    auto collectStats() noexcept -> void;
};

}

#endif // CLUBMOSS_DIS_COST_HXX
