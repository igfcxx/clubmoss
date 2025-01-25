#include "seq_cost.hxx"

namespace clubmoss::metric {

SeqCost::SeqCost(seq_cost::Data&& data) : data_(std::move(data)) {}

/**
 * @brief 计算组合代价
 * @param layout 输入的布局
 * @return 组合代价
*/
auto SeqCost::measure(const Layout& layout) -> fz {
    cost_ = 0.0;
    for (const Bigram& gm : data_.bigram_records_) {
        cost_ += cfg_.costOf(gm, layout) * gm.f;
    }
    for (const Trigram& gm : data_.trigram_records_) {
        cost_ += cfg_.costOf(gm, layout) * gm.f;
    }
    return cost_;
}

/**
 * @brief 计算组合代价, 检查有效性
 * @param layout 输入的布局
 * @return 组合代价
*/
auto SeqCost::analyze(const Layout& layout) -> fz {
    measure(layout);
    uz bad_ngram_count = 0;
    for (uz i = 0; i < cfg_.ngrams_to_test_; ++i) {
        if (cfg_.costOf(data_.bigram_records_[i], layout) > cfg_.max_pain_lvl_cost_) { ++bad_ngram_count; }
        if (cfg_.costOf(data_.trigram_records_[i], layout) > cfg_.max_pain_lvl_cost_) { ++bad_ngram_count; }
    }
    valid_ = bad_ngram_count <= cfg_.max_bad_ngram_count_;
    return cost_;
}

/**
 * @brief 记录统计数据
 * @param layout 输入的布局
 */
auto SeqCost::collectStats(const Layout& layout) -> void {
    uz bad_ngram_count = 0;
    cost_of_top_2_grams_.clear();
    cost_of_top_3_grams_.clear();
    for (uz i = 0; i < cfg_.ngrams_to_test_; ++i) {
        const uz bg_cost = cfg_.costOf(data_.bigram_records_[i], layout);
        cost_of_top_2_grams_.emplace_back(bg_cost);
        if (bg_cost > cfg_.max_pain_lvl_cost_) {
            ++bad_ngram_count;
        }
        const uz tg_cost = cfg_.costOf(data_.trigram_records_[i], layout);
        cost_of_top_3_grams_.emplace_back(tg_cost);
        if (tg_cost > cfg_.max_pain_lvl_cost_) {
            ++bad_ngram_count;
        }
    }
    valid_ = bad_ngram_count <= cfg_.max_bad_ngram_count_;
}

}
