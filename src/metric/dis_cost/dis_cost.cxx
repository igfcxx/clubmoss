#include "dis_cost.hxx"

namespace clubmoss::metric {

DisCost::DisCost(const dis_cost::Data& data) : data_(data) {}

/**
 * @brief 计算距离代价
 * @param layout 输入的布局
 * @return 距离代价
*/
auto DisCost::measure(const Layout& layout) -> fz {
    calcMovementsOfEachFinger(layout);
    cost_ = Utils::sum(finger_move_);
    return cost_;
}

/**
 * @brief 计算距离代价, 检查有效性
 * @param layout 输入的布局
 * @return 距离代价
*/
auto DisCost::analyze(const Layout& layout) -> std::pair<fz, uz> {
    calcMovementsOfEachFinger(layout);
    calcFingerUsage();
    validateUsage();
    return {cost_, flaw_count_};
}

/**
 * @brief 记录统计数据
 * @param layout 输入的布局
 * @param stats 待更新的统计数据
 * @return 距离代价, 缺陷数
 */
auto DisCost::scan(const Layout& layout, Toml& stats) -> std::pair<fz, uz> {
    calcMovementsOfEachFinger(layout);
    calcFingerUsage();
    collectStats();

    stats.at("finger_move") = finger_move_;
    stats.at("l_hand_move") = left_hand_usage_;
    stats.at("r_hand_move") = right_hand_usage_;
    stats.at("is_finger_dis_exceeded") = is_finger_overused_;
    stats.at("is_hand_dis_unbalanced") = is_hand_unbalanced_;

    return {cost_, flaw_count_};
}

auto DisCost::calcMovementsOfEachFinger(const Layout& layout) noexcept -> void {
    finger_move_.fill(0.0);
    for (const auto& op : data_.records_) {
        const Cap prev_cap = op.src;
        const Cap next_cap = op.dst;
        const fz freq = op.f;

        if (prev_cap != ' ' and next_cap != ' ') [[likely]] {
            updateUsage(layout, prev_cap, next_cap, freq);
        } else if (prev_cap == ' ') {
            updateUsage(layout, next_cap, freq);
        } else if (next_cap == ' ') {
            updateUsage(layout, prev_cap, freq);
        }
    }
}

auto base_position = [](const uz finger) -> uz {
    return finger + 10;
};

auto finger_to_press = [](const Pos pos) -> uz {
    const Col col = Utils::colOf(pos);
    if (col == 4) { return 3; }
    if (col == 5) { return 6; }
    return col;
};

auto DisCost::updateUsage(
    const Layout& layout,
    const Cap prev_cap,
    const Cap next_cap,
    const fz freq
) noexcept -> void {
    // 以 QWERTY 键盘为例
    // 从 C 到 E 只需左手中指移动一次
    // 而从 C 到 V 则可需要分别计算
    // 1. 左手中指从 C 到 D
    // 2. 左手食指从 F 到 V
    // 这两个动作的移动距离

    // @formatter:off //
    const Pos prev_pos = layout.getPos(prev_cap);
    const Pos next_pos = layout.getPos(next_cap);
    const uz prev_fin  = finger_to_press(prev_pos);
    const uz next_fin  = finger_to_press(next_pos);
    // @formatter:on //

    if (prev_fin != next_fin) {
        // prev finger: prev key -> it's base key
        // next finger: next key -> it's base key
        const Pos prev_fin_curr_pos = base_position(prev_fin);
        const Pos next_fin_curr_pos = base_position(next_fin);
        finger_move_[prev_fin] += cfg_.disBetween(prev_fin_curr_pos, prev_pos) * freq;
        finger_move_[next_fin] += cfg_.disBetween(next_fin_curr_pos, next_pos) * freq;
    } else {
        // the only finger: prev key -> next key
        finger_move_[prev_fin] += cfg_.disBetween(prev_pos, next_pos) * freq;
    }
}

auto DisCost::updateUsage(const Layout& layout, const Cap cap, const fz freq) noexcept -> void {
    // @formatter:off //
    const Pos pos = layout.getPos(cap);
    const uz  fin = finger_to_press(pos);
    const Pos base_pos = base_position(fin);
    finger_move_[fin] += cfg_.disBetween(pos, base_pos) * freq;
    // @formatter:on //
}

auto DisCost::calcFingerUsage() noexcept -> void {
    cost_ = Utils::sum(finger_move_);
    for (const Finger fin : Finger::_values()) {
        finger_usage_[fin] = finger_move_[fin] / cost_;
    }
}

auto DisCost::validateUsage() noexcept -> void {
    flaw_count_ = 0;

    // 检查每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        if (finger_usage_[fin] > cfg_.max_finger_mov_[fin]) {
            ++flaw_count_;
        }
    }

    // 检查左右手使用是否均衡
    fz left_hand_usage = 0.0;
    for (uz i = 0; i < 5; ++i) { left_hand_usage += finger_usage_[i]; }
    const fz deviation = std::abs(left_hand_usage - 0.5);
    if (deviation > cfg_.max_hand_mov_imbalance_) {
        ++flaw_count_;
    }
}

auto DisCost::collectStats() noexcept -> void {
    flaw_count_ = 0;

    // 检查并记录每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        is_finger_overused_[fin] = finger_usage_[fin] > cfg_.max_finger_mov_[fin];
        if (is_finger_overused_[fin]) { ++flaw_count_; }
    }

    // 记录左、右手使用率
    left_hand_usage_ = std::accumulate(
        &finger_usage_[Finger::LeftPinky],
        &finger_usage_[Finger::LeftThumb],
        0.0
    );
    right_hand_usage_ = 1.0 - left_hand_usage_;

    // 检查并记录左右手使用是否均衡
    const fz deviation = std::abs(left_hand_usage_ - 0.5);
    is_hand_unbalanced_ = deviation > cfg_.max_hand_mov_imbalance_;
    if (is_hand_unbalanced_) {
        ++flaw_count_;
    }
}

}
