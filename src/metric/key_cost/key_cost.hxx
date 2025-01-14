#ifndef CLUBMOSS_KEY_COST_HXX
#define CLUBMOSS_KEY_COST_HXX

#include "key_cost_config.hxx"

namespace clubmoss::metric {

// 击键代价指标 //
class KeyCost {
public:
    explicit KeyCost(const key_cost::Data& data);

    KeyCost() = delete;

    auto analyze(const Layout&) -> void;
    auto check(const Layout&) -> bool;

    auto measure1(const Layout&) -> fz;
    auto measure2(const Layout&) -> fz;
    auto measure3(const Layout&) -> fz;

    static auto loadCfg(const Toml& cfg) -> void;

protected:
    fz cost_{0.0};      // 代价分数
    bool valid_{false}; // 是否有效

    std::array<fz, KEY_COUNT> heat_map_{0.0};  // 热力图
    std::array<fz, ROW_COUNT> row_usage_{0.0}; // 行使用率
    std::array<fz, COL_COUNT> col_usage_{0.0}; // 列使用率

    std::array<fz, Finger::_size()> finger_usage_{0.0};         // 手指使用率
    std::array<bool, Finger::_size()> is_finger_overused_{true}; // 手指使用是否过高

    fz left_hand_usage_{0.5};      // 左手使用率
    fz right_hand_usage_{0.5};     // 右手使用率
    bool is_hand_unbalanced_{true}; // 左右手使用是否不均衡

    key_cost::Data data_; // 待测数据

    auto calcFingerUsage() noexcept -> void;

private:
    inline static key_cost::Config cfg_; // 配置信息

    auto validateUsage() noexcept -> void;
    auto collectStats() noexcept -> void;
};

}

#endif // CLUBMOSS_KEY_COST_HXX
