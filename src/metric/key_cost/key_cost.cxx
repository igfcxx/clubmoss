#include "key_cost.hxx"

namespace clubmoss::metric {

KeyCost::KeyCost(const key_cost::Data& data) : data_(data) {}

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
auto KeyCost::analyze(const Layout& layout) -> std::pair<fz, uz> {
    calcFingerUsage(layout);
    validateLayout();
    return {cost_, flaw_count_};
}

/**
 * @brief 记录统计数据
 * @param layout 输入的布局
 * @param stats 待更新的统计数据
 * @return 击键代价, 缺陷数
 */
auto KeyCost::scan(const Layout& layout, Toml& stats) -> std::pair<fz, uz> {
    calcFingerUsage(layout);
    collectStats();

    stats.at("heat_map") = heat_map_;
    stats.at("row_usage") = row_usage_;
    stats.at("col_usage") = col_usage_;
    stats.at("l_hand_usage") = left_hand_usage_;
    stats.at("r_hand_usage") = right_hand_usage_;
    stats.at("is_finger_hit_exceeded") = is_finger_overused_;
    stats.at("is_hand_hit_unbalanced") = is_hand_unbalanced_;
    stats.at("similarity") = similarity_;

    return {cost_, flaw_count_};
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
    flaw_count_ = 0;
    // 检查每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        if (finger_usage_[fin] > cfg_.max_finger_use_[fin]) {
            ++flaw_count_;
        }
    }
    // 检查左右手使用是否均衡
    fz left_hand_usage = 0.0;
    for (uz i = 0; i < 5; ++i) { left_hand_usage += finger_usage_[i]; }
    const fz deviation = std::abs(left_hand_usage - 0.5);
    if (deviation > cfg_.max_hand_use_imbalance_) {
        ++flaw_count_;
    }
}

auto KeyCost::collectStats() noexcept -> void {
    flaw_count_ = 0;

    // 检查并记录每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        is_finger_overused_[fin] = finger_usage_[fin] > cfg_.max_finger_use_[fin];
        if (is_finger_overused_[fin]) { ++flaw_count_; }
    }

    // 记录左、手使用率
    left_hand_usage_ = std::accumulate(
        &finger_usage_[Finger::LeftPinky],
        &finger_usage_[Finger::LeftThumb],
        0.0
    );
    right_hand_usage_ = 1.0 - left_hand_usage_;

    // 检查并记录左右手使用是否均衡
    const fz deviation = std::abs(left_hand_usage_ - 0.5);
    is_hand_unbalanced_ = deviation > cfg_.max_hand_use_imbalance_;
    if (is_hand_unbalanced_) {
        ++flaw_count_;
    }
}

}
