#include "key_cost.hxx"

namespace clubmoss::metric {

KeyCost::KeyCost(const key_cost::Data& data) : data_(data) {}

auto KeyCost::loadCfg(const Toml& cfg) -> void { cfg_.loadCfg(cfg); }

/**
 * @brief 计算击键成本, 检查有效性, 并记录其他统计信息
 * @param layout 输入的布局
 */
auto KeyCost::analyze(const Layout& layout) -> void {
    cost_ = 0.0;
    heat_map_.fill(0.0);
    row_usage_.fill(0.0);
    col_usage_.fill(0.0);
    finger_usage_.fill(0.0);
    for (const Cap cap : CAP_SET) {
        const fz freq = data_.char_freq_[cap];
        const Pos pos = layout.getPos(cap);
        const Col col = Utils::colOf(pos);
        const Row row = Utils::rowOf(pos);
        cost_ += cfg_.costs_[pos] * freq;
        col_usage_[col] += freq;
        row_usage_[row] += freq;
        heat_map_[pos] += freq;
    }
    calcFingerUsage();
    collectStats();
}

/**
 * @brief 计算击键成本
 * @param layout 输入的布局
 * @return 击键成本
 */
auto KeyCost::measure(const Layout& layout) -> fz {
    cost_ = 0.0;
    for (const Cap cap : CAP_SET) {
        const fz freq = data_.char_freq_[cap];
        const Pos pos = layout.getPos(cap);
        cost_ += cfg_.costs_[pos] * freq;
    }
    return cost_;
}

/**
 * @brief 检查布局是否有效
 * @param layout 输入的布局
 * @return 布局是否有效
 */
auto KeyCost::check(const Layout& layout) -> bool {
    col_usage_.fill(0.0);
    for (const Cap cap : CAP_SET) {
        const fz freq = data_.char_freq_[cap];
        const Pos pos = layout.getPos(cap);
        const Col col = Utils::colOf(pos);
        col_usage_[col] += freq;
    }
    calcFingerUsage();
    validateUsage();
    return valid_;
}

auto KeyCost::calcFingerUsage() noexcept -> void {
    finger_usage_ = col_usage_;
    // 根据手指使用率与列使用率的对应关系调整元素
    finger_usage_[Finger::LeftThumb]  = 0.0;
    finger_usage_[Finger::LeftIndex]  = col_usage_[3] + col_usage_[4];
    finger_usage_[Finger::RightIndex] = col_usage_[5] + col_usage_[6];
    finger_usage_[Finger::RightThumb] = 0.0;
}

auto KeyCost::validateUsage() noexcept -> void {
    // 检查每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        if (finger_usage_[fin] > cfg_.max_finger_usage_[fin]) {
            valid_ = false;
            return;
        }
    }
    // 检查左右手使用是否均衡
    fz left_hand_usage = 0.0;
    for (uz i = 0; i < 5; ++i) { left_hand_usage += col_usage_[i]; }
    valid_ = std::abs(left_hand_usage - 0.5) <= cfg_.max_hand_usage_imbalance_;
}

auto KeyCost::collectStats() noexcept -> void {
    // 检查并记录每个手指使用率是否超过限制
    for (const Finger fin : Finger::_values()) {
        is_finger_overused_[fin] = finger_usage_[fin] > cfg_.max_finger_usage_[fin];
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
    const fz deviation  = std::abs(left_hand_usage_ - 0.5);
    is_hand_unbalanced_ = deviation > cfg_.max_hand_usage_imbalance_;

    // 记录布局整体是否有效
    valid_ = not(is_hand_unbalanced_ or std::ranges::any_of(
        is_finger_overused_, [](const bool elem) -> bool {
            return elem;
        }
    ));
}

}
