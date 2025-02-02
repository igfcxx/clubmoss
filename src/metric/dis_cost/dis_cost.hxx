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

    auto calcFingerMovement(const Layout& layout) noexcept -> void;

    auto calcAndVerifyFingerUsage() noexcept -> void;

    dis_cost::Data data_;

private:
    inline static Config& cfg_ = Config::getInstance();

    auto updateUsage(const Layout&, Cap prev_cap, Cap next_cap, fz freq) noexcept -> void;
    auto updateUsage(const Layout&, Cap cap, fz freq) noexcept -> void;

    friend class clubmoss::Evaluator;
};

}

#endif // CLUBMOSS_DIS_COST_HXX
