#ifndef CLUBMOSS_SEQ_COST_HXX
#define CLUBMOSS_SEQ_COST_HXX

#include "seq_cost_config.hxx"

namespace clubmoss::metric {

// 击键代价指标 //
class SeqCost {
public:
    explicit SeqCost(const seq_cost::Data& data);

    SeqCost() = delete;

    // auto analyze(const Layout&) -> void;
    // auto check(const Layout&) -> bool;

    auto measure(const Layout&) -> fz;

    static auto loadCfg(const Toml& cfg) -> void;

protected:
    fz cost_{0.0};      // 代价分数
    bool valid_{false}; // 是否有效

    std::array<fz, PainLevel::_size()> lvl_freq_;

    seq_cost::Data data_; // 待测数据

private:
    inline static seq_cost::Config cfg_; // 配置信息

    // auto collectStats() noexcept -> void;
};

}

#endif // CLUBMOSS_SEQ_COST_HXX
