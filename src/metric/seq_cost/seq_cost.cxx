#include "seq_cost.hxx"

namespace clubmoss::metric {

SeqCost::SeqCost(const seq_cost::Data& data): data_(data) {}

auto SeqCost::loadCfg(const Toml& cfg) -> void { cfg_.loadCfg(cfg); }

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

}
