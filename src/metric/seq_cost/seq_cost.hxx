#ifndef CLUBMOSS_SEQ_COST_HXX
#define CLUBMOSS_SEQ_COST_HXX

#include "seq_cost_data.hxx"

namespace clubmoss {
class Analyzer;
}

namespace clubmoss::metric {

// 组合代价指标 //
class SeqCost {
public:
    explicit SeqCost(seq_cost::Data&& data);

    auto analyze(const Layout&) -> fz;
    auto measure(const Layout&) -> fz;
    auto collectStats(const Layout&) -> void;

    SeqCost() = delete;

protected:
    fz cost_{0.0}; // 代价分数
    bool valid_{false}; // 是否有效

    std::vector<uz> cost_of_top_2_grams_; // 最常用的 2-gram 的不适程度
    std::vector<uz> cost_of_top_3_grams_; // 最常用的 3-gram 的不适程度

private:
    inline static Config& cfg_ = Config::getInstance();

    seq_cost::Data data_;

    friend class clubmoss::Analyzer;
};

}

#endif // CLUBMOSS_SEQ_COST_HXX
