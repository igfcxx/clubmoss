#ifndef CLUBMOSS_SEQ_COST_HXX
#define CLUBMOSS_SEQ_COST_HXX

#include "seq_cost_data.hxx"

namespace clubmoss::metric {

// 组合代价指标 //
class SeqCost {
public:
    explicit SeqCost(const seq_cost::Data& data);

    auto measure(const Layout&) -> fz;
    auto analyze(const Layout&) -> std::pair<fz, uz>;
    auto scan(const Layout&, Toml& stats) -> std::pair<fz, uz>;

    SeqCost() = delete;

protected:
    fz cost_{0.0}; // 代价分数
    uz flaw_count_{0}; // 缺陷数

    std::vector<uz> pain_level_of_top_2_grams_; // 最常用的 2-gram 的不适程度
    std::vector<uz> pain_level_of_top_3_grams_; // 最常用的 3-gram 的不适程度

private:
    inline static Config& cfg_ = Config::getInstance();

    seq_cost::Data data_;

    friend class clubmoss::Evaluator;
};

}

#endif // CLUBMOSS_SEQ_COST_HXX
