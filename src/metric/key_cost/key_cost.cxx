#include "key_cost.hxx"

namespace clubmoss::metric {

KeyCost::KeyCost(key_cost::Data&& data) : data_(std::move(data)) {}

/**
 * @brief 计算击键代价
 * @param layout 输入的布局
 * @return 击键代价
 */
auto KeyCost::measure(const Layout& layout) -> fz {
    cost_ = 0.0;
    for (uz i = 0; i < KEY_COUNT; i += 2) {
        const fz freq1 = data_.freq_[i];
        const fz freq2 = data_.freq_[i + 1];
        const Pos pos1 = layout.getPos(data_.caps_[i]);
        const Pos pos2 = layout.getPos(data_.caps_[i + 1]);
        cost_ += cfg_.key_costs_[pos1] * freq1 + cfg_.key_costs_[pos2] * freq2;
    }
    return cost_;
}

/**
 * @brief 计算击键代价, 检查有效性
 * @param layout 输入的布局
 * @return 击键代价
 */
auto KeyCost::analyze(const Layout& layout) -> fz {
    calcFingerUsage(layout);
    validateLayout();
    return cost_;
}

/**
 * @brief 记录统计数据
 * @param layout 输入的布局
 */
auto KeyCost::collectStats(const Layout& layout) -> void {
    calcFingerUsage(layout);
    analyze(layout);
    collectStats();
}

auto is_same_finger = [](const Col col1, const Col col2) -> bool {
    if (col1 == col2) {
        return true;
    } else if (col1 == 3 or col1 == 4) {
        return col2 == 3 or col2 == 4;
    } else if (col1 == 5 or col1 == 6) {
        return col2 == 5 or col2 == 6;
    }
    return false;
};

auto is_same_hand = [](const Col col1, const Col col2) -> bool {
    return (col1 <= 4 and col2 <= 4) or (col1 >= 5 and col2 >= 5);
};

auto KeyCost::calcFingerUsage(const Layout& layout) noexcept -> void {
    static auto ref = layout::baselines::QWERTY;

    cost_ = 0.0;
    heat_map_.fill(0.0);
    row_usage_.fill(0.0);
    col_usage_.fill(0.0);
    finger_usage_.fill(0.0);

    for (uz i = 0; i < KEY_COUNT; ++i) {
        const fz freq = data_.freq_[i];
        const Cap cap = data_.caps_[i];
        const Pos ref_pos = ref.getPos(cap);
        const Pos cur_pos = layout.getPos(cap);
        const Col ref_col = Utils::colOf(ref_pos);
        const Col cur_col = Utils::colOf(cur_pos);
        const Row cur_row = Utils::rowOf(cur_pos);

        cost_ += cfg_.key_costs_[cur_pos] * freq;
        col_usage_[cur_col] += freq;
        row_usage_[cur_row] += freq;
        heat_map_[cur_pos] += freq;

        if (cur_pos == ref_pos) {
            similarity_ += cfg_.similarity_score_[PosRelation::SamePosition] * freq;
        } else if (is_same_finger(ref_col, cur_col)) {
            similarity_ += cfg_.similarity_score_[PosRelation::SameFinger] * freq;
        } else if (is_same_hand(ref_col, cur_col)) {
            similarity_ += cfg_.similarity_score_[PosRelation::SameHand] * freq;
        }
    }

    // @formatter:off //
    finger_usage_ = col_usage_;
    finger_usage_[Finger::LeftThumb]  = 0.0;
    finger_usage_[Finger::RightThumb] = 0.0;
    finger_usage_[Finger::LeftIndex]  = col_usage_[3] + col_usage_[4];
    finger_usage_[Finger::RightIndex] = col_usage_[5] + col_usage_[6];
    // @formatter:on //
}

auto KeyCost::validateLayout() noexcept -> void {
    // 检查每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        if (finger_usage_[fin] > cfg_.max_finger_use_[fin]) {
            valid_ = false;
            return;
        }
    }
    // 检查左右手使用是否均衡
    fz left_hand_usage = 0.0;
    for (uz i = 0; i < 5; ++i) { left_hand_usage += finger_usage_[i]; }
    valid_ = std::abs(left_hand_usage - 0.5) <= cfg_.max_hand_use_imbalance_;
}

auto KeyCost::collectStats() noexcept -> void {
    valid_ = true;

    // 检查并记录每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        is_finger_overused_[fin] = finger_usage_[fin] > cfg_.max_finger_use_[fin];
        if (is_finger_overused_[fin]) { valid_ = false; }
    }

    // 记录左手使用率
    left_hand_usage_ = std::accumulate(
        &finger_usage_[Finger::LeftPinky],
        &finger_usage_[Finger::LeftThumb],
        0.0
    );
    // 记录右手使用率
    right_hand_usage_ = 1.0 - left_hand_usage_;

    // 检查并记录左右手使用是否均衡
    const fz deviation = std::abs(left_hand_usage_ - 0.5);
    is_hand_unbalanced_ = deviation > cfg_.max_hand_use_imbalance_;

    // 记录布局整体是否有效
    valid_ = valid_ and not is_hand_unbalanced_;
}

}
