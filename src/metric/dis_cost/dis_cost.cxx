#include "dis_cost.hxx"
#include<iostream>

namespace clubmoss::metric {

DisCost::DisCost(const dis_cost::Data& data): data_(data) {}

auto DisCost::loadCfg(const Toml& cfg) -> void { cfg_.loadCfg(cfg); }

auto DisCost::measure(const Layout& layout) -> fz {
    finger_move_.fill(0.0);
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
    cost_ = std::accumulate(finger_move_.begin(), finger_move_.end(), 0.0);
    return cost_;
}

auto finger_to_press = [](const Pos pos) -> uz {
    const Col col = Utils::colOf(pos);
    if (col == 4) { return 3; }
    if (col == 5) { return 6; }
    return col;
};

auto DisCost::updateUsage(
    const Layout& layout, const Cap prev_cap, const Cap next_cap, const fz freq
) noexcept -> void {
    const Pos prev_pos = layout.getPos(prev_cap);
    const Pos next_pos = layout.getPos(next_cap);
    const uz prev_fin  = finger_to_press(prev_pos);
    const uz next_fin  = finger_to_press(next_pos);

    if (prev_fin != next_fin) {
        // prev finger: prev key -> it's base key
        // next finger: next key -> it's base key
        const Pos prev_fin_curr_pos = cfg_.base_pos_[prev_fin];
        const Pos next_fin_curr_pos = cfg_.base_pos_[next_fin];
        finger_move_[prev_fin] += cfg_.disBetween(prev_fin_curr_pos, prev_pos) * freq;
        finger_move_[next_fin] += cfg_.disBetween(next_fin_curr_pos, next_pos) * freq;
    } else {
        // the only finger: prev key -> next key
        finger_move_[prev_fin] += cfg_.disBetween(prev_pos, next_pos) * freq;
    }
}

auto DisCost::updateUsage(const Layout& layout, const Cap cap, const fz freq) noexcept -> void {
    const Pos pos = layout.getPos(cap);
    const uz fin  = finger_to_press(pos);
    finger_move_[fin] += cfg_.disBetween(pos, cfg_.base_pos_[fin]) * freq;
}

}
