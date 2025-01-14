#include "dis_cost.hxx"
#include<iostream>

namespace clubmoss::metric {

DisCost::DisCost(const dis_cost::Data& data): data_(data) {}

auto DisCost::loadCfg(const Toml& cfg) -> void { cfg_.loadCfg(cfg); }

auto DisCost::measure1(const Layout& layout) -> fz {
    finger_usage_.fill(0.0);
    for (const auto& op : data_.records_) {
        const Cap prev_cap = op.src;
        const Cap next_cap = op.dst;
        const fz freq      = op.f;

        if (prev_cap != ' ' and next_cap != ' ') [[likely]] {
            updateUsage(layout, prev_cap, next_cap, freq);
        } else if (prev_cap == ' ') {
            updateUsage(layout, next_cap, freq);
        } else if (next_cap == ' ') {
            updateUsage(layout, prev_cap, freq);
        }
    }
    cost_ = std::accumulate(finger_usage_.begin(), finger_usage_.end(), 0.0);
    return cost_;
}

auto DisCost::measure2(const Layout& layout) -> fz {
    finger_usage_.fill(0.0);
    for (uz i = 0; i < data_.SIZE; ++i) {
        const Cap prev_cap = data_.c1_[i][0];
        const Cap next_cap = data_.c1_[i][1];
        const fz freq      = data_.freq_[i];

        if (prev_cap != ' ' and next_cap != ' ') [[likely]] {
            updateUsage(layout, prev_cap, next_cap, freq);
        } else if (prev_cap == ' ') {
            updateUsage(layout, next_cap, freq);
        } else if (next_cap == ' ') {
            updateUsage(layout, prev_cap, freq);
        }
    }
    cost_ = std::accumulate(finger_usage_.begin(), finger_usage_.end(), 0.0);
    return cost_;
}

auto DisCost::measure3(const Layout& layout) -> fz {
    finger_usage_.fill(0.0);
    for (uz i = 0; i < data_.SIZE; ++i) {
        const Cap prev_cap = data_.c2_[0][i];
        const Cap next_cap = data_.c2_[1][i];
        const fz freq      = data_.freq_[i];

        if (prev_cap != ' ' and next_cap != ' ') [[likely]] {
            updateUsage(layout, prev_cap, next_cap, freq);
        } else if (prev_cap == ' ') {
            updateUsage(layout, next_cap, freq);
        } else if (next_cap == ' ') {
            updateUsage(layout, prev_cap, freq);
        }
    }
    cost_ = std::accumulate(finger_usage_.begin(), finger_usage_.end(), 0.0);
    return cost_;
}

auto DisCost::updateUsage(const Layout& layout, Cap prev_cap, Cap next_cap, fz freq) noexcept -> void {
    const Pos prev_pos  = layout.getPos(prev_cap);
    const Pos next_pos  = layout.getPos(next_cap);
    const auto prev_fin = Finger::_from_index(Utils::colOf(prev_pos));
    const auto next_fin = Finger::_from_index(Utils::colOf(next_pos));

    if (prev_fin != next_fin) {
        Pos curr_pos;
        // prev finger: prev key -> it's base key
        curr_pos = cfg_.base_pos_[prev_fin];
        finger_usage_[prev_fin] += cfg_.disBetween(curr_pos, prev_pos) * freq;
        // next finger: next key -> it's base key
        curr_pos = cfg_.base_pos_[next_fin];
        finger_usage_[next_fin] += cfg_.disBetween(curr_pos, next_pos) * freq;
    } else {
        // the only finger: prev key -> next key
        finger_usage_[prev_fin] += cfg_.disBetween(prev_pos, next_pos) * freq;
    }
}

auto DisCost::updateUsage(const Layout& layout, Cap cap, fz freq) noexcept -> void {
    const Pos pos  = layout.getPos(cap);
    const auto fin = Finger::_from_index(Utils::colOf(pos));
    finger_usage_[fin] += cfg_.disBetween(pos, cfg_.base_pos_[pos]) * freq;
}

}
