#include "seq_cost.hxx"

namespace clubmoss::metric {

SeqCost::SeqCost(const seq_cost::Data& data) : data_(data) {}

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
auto SeqCost::analyze(const Layout& layout) -> std::pair<fz, uz> {
    cost_ = 0.0;
    flaw_count_ = 0;
    // 考察 2-gram 记录
    for (const auto& bigram : data_.bigram_records_ | std::views::take(cfg_.ngrams_to_test_)) {
        const uz cost = cfg_.costOf(bigram, layout);
        if (cost > cfg_.max_ngram_cost_) { ++flaw_count_; }
        cost_ += cost * bigram.f;
    }
    for (const auto& bigram : data_.bigram_records_ | std::views::drop(cfg_.ngrams_to_test_)) {
        cost_ += cfg_.costOf(bigram, layout) * bigram.f;
    }
    // 考察 3-gram 记录
    for (const auto& trigram : data_.trigram_records_ | std::views::take(cfg_.ngrams_to_test_)) {
        const uz cost = cfg_.costOf(trigram, layout);
        if (cost > cfg_.max_ngram_cost_) { ++flaw_count_; }
        cost_ += cost * trigram.f;
    }
    for (const auto& trigram : data_.trigram_records_ | std::views::drop(cfg_.ngrams_to_test_)) {
        cost_ += cfg_.costOf(trigram, layout) * trigram.f;
    }
    return {cost_, flaw_count_};
}

/**
 * @brief 记录统计数据
 * @param layout 输入的布局
 * @param stats 待更新的统计数据
 * @return 组合代价, 缺陷数
 */
auto SeqCost::scan(const Layout& layout, Toml& stats) -> std::pair<fz, uz> {
    cost_ = 0.0;
    flaw_count_ = 0;
    pain_level_of_top_2_grams_.clear();
    pain_level_of_top_3_grams_.clear();
    // 考察 2-gram 记录
    for (const auto& bigram : data_.bigram_records_ | std::views::take(cfg_.ngrams_to_test_)) {
        const uz cost = cfg_.costOf(bigram, layout);
        const uz pain = cfg_.painLevelOf(bigram, layout);
        if (cost > cfg_.max_ngram_cost_) { ++flaw_count_; }
        pain_level_of_top_2_grams_.emplace_back(pain);
        cost_ += cost * bigram.f;
    }
    for (const auto& bigram : data_.bigram_records_ | std::views::drop(cfg_.ngrams_to_test_)) {
        cost_ += cfg_.costOf(bigram, layout) * bigram.f;
    }
    // 考察 3-gram 记录
    for (const auto& trigram : data_.trigram_records_ | std::views::take(cfg_.ngrams_to_test_)) {
        const uz cost = cfg_.costOf(trigram, layout);
        const uz pain = cfg_.painLevelOf(trigram, layout);
        if (cost > cfg_.max_ngram_cost_) { ++flaw_count_; }
        pain_level_of_top_3_grams_.emplace_back(pain);
        cost_ += cost * trigram.f;
    }
    for (const auto& trigram : data_.trigram_records_ | std::views::drop(cfg_.ngrams_to_test_)) {
        cost_ += cfg_.costOf(trigram, layout) * trigram.f;
    }

    stats.at("2_gram_pain_levels") = pain_level_of_top_2_grams_;
    stats.at("3_gram_pain_levels") = pain_level_of_top_3_grams_;

    return {cost_, flaw_count_};
}

}
