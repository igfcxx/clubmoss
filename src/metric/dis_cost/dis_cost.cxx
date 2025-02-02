#include "dis_cost.hxx"

namespace clubmoss::metric {

DisCost::DisCost(const dis_cost::Data& data) : data_(data) {}

/**
 * @brief 计算距离代价
 * @param layout 输入的布局
 * @return 距离代价
*/
auto DisCost::measure(const Layout& layout) -> fz {
    calcFingerMovement(layout);
    cost_ = Utils::sum(finger_move_);
    return cost_;
}

/**
 * @brief 计算距离代价, 检查有效性
 * @param layout 输入的布局
 * @return 距离代价
*/
auto DisCost::analyze(const Layout& layout) -> std::pair<fz, uz> {
    calcFingerMovement(layout);
    calcAndVerifyFingerUsage();
    return {cost_, flaw_count_};
}

/**
 * @brief 记录统计数据
 * @param layout 输入的布局
 * @param stats 待更新的统计数据
 * @return 距离代价, 缺陷数
 */
auto DisCost::scan(const Layout& layout, Toml& stats) -> std::pair<fz, uz> {
    calcFingerMovement(layout);
    calcAndVerifyFingerUsage();
    stats.at("finger_movement") = finger_usage_;
    return {cost_, flaw_count_};
}

auto DisCost::calcFingerMovement(const Layout& layout) noexcept -> void {
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

auto finger_to_hit = [](const Pos pos) -> uz {
    const Col col = Utils::colOf(pos);
    if (col == 4) [[unlikely]] return 3;
    if (col == 5) [[unlikely]] return 6;
    return col;
};

auto DisCost::updateUsage(
    const Layout& layout,
    const Cap prev_cap,
    const Cap next_cap,
    const fz freq
) noexcept -> void {
    const Pos prev_pos = layout.getPos(prev_cap);
    const Pos next_pos = layout.getPos(next_cap);
    const uz prev_fin = finger_to_hit(prev_pos);
    const uz next_fin = finger_to_hit(next_pos);

    // 以 QWERTY 键盘为例
    if (prev_fin != next_fin) {
        // C -> V 涉及以下两个动作
        // 1. 左手中指从 C 到 D
        // 2. 左手食指从 F 到 V
        // 需要分别计算这两个动作的距离
        const Pos prev_fin_curr_pos = base_position(prev_fin);
        const Pos next_fin_curr_pos = base_position(next_fin);
        finger_move_[prev_fin] += cfg_.disBetween(prev_fin_curr_pos, prev_pos) * freq;
        finger_move_[next_fin] += cfg_.disBetween(next_fin_curr_pos, next_pos) * freq;
    } else {
        // 而 C -> E 只需计算左手中指的移动距离
        finger_move_[prev_fin] += cfg_.disBetween(prev_pos, next_pos) * freq;
    }
}

auto DisCost::updateUsage(
    const Layout& layout,
    const Cap cap,
    const fz freq
) noexcept -> void {
    // @formatter:off //
    const Pos pos = layout.getPos(cap);
    const uz  fin = finger_to_hit(pos);
    const Pos base_pos = base_position(fin);
    finger_move_[fin] += cfg_.disBetween(pos, base_pos) * freq;
    // @formatter:on //
}

auto DisCost::calcAndVerifyFingerUsage() noexcept -> void {
    // 将移动距离除以总距离, 转化为使用率
    cost_ = Utils::sum(finger_move_);
    for (const Finger fin : Finger::_values()) {
        finger_usage_[fin] = finger_move_[fin] / cost_;
    }

    flaw_count_ = 0;
    // 检查每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        if (finger_usage_[fin] > cfg_.max_finger_mov_[fin]) {
            ++flaw_count_;
        }
    }
    // 检查左右手使用是否均衡
    const fz left_hand_usage = std::accumulate(
        &finger_usage_[Finger::LeftPinky],
        &finger_usage_[Finger::LeftThumb],
        0.0
    );
    const fz deviation = std::abs(left_hand_usage - 0.5);
    if (deviation > cfg_.max_hand_mov_imbalance_) {
        ++flaw_count_;
    }
}

}
