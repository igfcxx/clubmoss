#ifndef CLUBMOSS_SEQ_COST_HXX
#define CLUBMOSS_SEQ_COST_HXX

#include "seq_cost_config.hxx"

namespace clubmoss {
class Analyzer;
}

namespace clubmoss::metric {

// 序列代价指标 //
class SeqCost {
public:
    auto analyze(const Layout&, const seq_cost::Data& data) -> void;
    auto measure(const Layout&, const seq_cost::Data& data) -> fz;
    auto check(const Layout&, const seq_cost::Data& data) -> bool;

    static auto loadCfg(const Toml& cfg) -> void;

protected:
    fz cost_{0.0};      // 代价分数
    bool valid_{false}; // 是否有效

    std::vector<uz> pain_lvl_of_top_2_grams_; // 最常用的 2-gram 的不适程度
    std::vector<uz> pain_lvl_of_top_3_grams_; // 最常用的 3-gram 的不适程度

private:
    inline static seq_cost::Config& cfg_ = seq_cost::Config::getInstance();

    friend class clubmoss::Analyzer;
};

}

#endif // CLUBMOSS_SEQ_COST_HXX
