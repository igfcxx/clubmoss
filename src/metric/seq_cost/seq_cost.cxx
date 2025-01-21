#include "seq_cost.hxx"

namespace clubmoss::metric {

SeqCost::SeqCost(const seq_cost::Data& data): data_(data) {}

auto SeqCost::loadCfg(const Toml& cfg) -> void { cfg_.loadCfg(cfg); }

auto SeqCost::analyze(const Layout& layout) -> void {
    measure(layout);
    for (uz i = 0; i < cfg_.top_ngrams_; ++i) {
        pain_lvl_of_top_2_grams_.emplace_back(cfg_.painLvlOf(data_.bigram_records_[i], layout));
        pain_lvl_of_top_3_grams_.emplace_back(cfg_.painLvlOf(data_.trigram_records_[i], layout));
    }
}

auto SeqCost::measure(const Layout& layout) -> fz {
    cost_ = 0.0;
    for (const seq_cost::Bigram& bigram : data_.bigram_records_) {
        cost_ += cfg_.costOf(bigram, layout) * bigram.f;
    }
    for (const seq_cost::Trigram& trigram : data_.trigram_records_) {
        cost_ += cfg_.costOf(trigram, layout) * trigram.f;
    }
    return cost_;
}

auto SeqCost::check(const Layout& layout) -> bool {
    uz bad_ngram_count = 0;
    for (uz i = 0; i < cfg_.top_ngrams_; ++i) {
        if (cfg_.painLvlOf(data_.bigram_records_[i], layout) >= PainLevel::Severe) {
            ++bad_ngram_count;
        }
        if (cfg_.painLvlOf(data_.trigram_records_[i], layout) >= PainLevel::Severe) {
            ++bad_ngram_count;
        }
    }
    return bad_ngram_count <= cfg_.bad_allowed_;
}

}
