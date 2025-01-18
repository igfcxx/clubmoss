#include "dis_cost_config.hxx"

namespace clubmoss::metric::dis_cost {

auto Config::getInstance() -> Config& {
    static Config instance;
    return instance;
}

auto Config::disBetween(const Pos pos1, const Pos pos2) const -> fz {
    return dis_[pos1 * KEY_CNT_POW2 + pos2];
}

auto Config::loadCfg(const Toml& cfg) -> void {
    loadFinLimits(cfg);
    loadHandLimit(cfg);
    clacDistance();
}

auto Config::loadFinLimits(const Toml& cfg) -> void {
    const Toml& node = cfg.at("max_finger_usage");
    // [最大手指使用率]数组的长度必须等于手指的数量
    if (node.size() != Finger::_size()) {
        throw IllegalCfg(
            "illegal size of `max_finger_usage`",
            node, std::format("should be {}", Finger::_size())
        );
    }
    // [最大手指使用率]数组的元素必须是浮点数，且在 (0, 0.35] 之间
    for (const auto& [fin,elem] : node.as_array() | std::views::enumerate) {
        if (fin == Finger::LeftThumb or fin == Finger::RightThumb) continue;
        if (const double limit = elem.as_floating();
            limit < 0.0 or limit > 0.35) {
            throw IllegalCfg(
                "illegal finger usage limit",
                elem, "should be in range (0, 0.35]"
            );
        } else {
            max_finger_usage_[fin] = static_cast<fz>(limit);
        }
    }
}

auto Config::loadHandLimit(const Toml& cfg) -> void {
    // [最大左右手偏移量]必须是一个浮点数，且在 (0, 0.15] 之间
    const Toml& node = cfg.at("max_hand_usage_imbalance");
    if (const double max_hand_usage_imbalance = node.as_floating();
        max_hand_usage_imbalance < 0.0 or max_hand_usage_imbalance > 0.15) {
        throw IllegalCfg(
            "illegal max finger usage",
            node, "should be in range (0, 0.1]"
        );
    } else {
        max_hand_usage_imbalance_ = static_cast<fz>(max_hand_usage_imbalance);
    }
}

auto Config::clacDistance() -> void {
    for (const Pos pos1 : POS_SET) {
        const fz x1 = x_[pos1];
        const fz y1 = y_[pos1];

        for (const Pos pos2 : POS_SET) {
            const fz x2 = x_[pos2];
            const fz y2 = y_[pos2];

            const fz dis = std::hypot(x1 - x2, y1 - y2);

            dis_[pos1 * KEY_CNT_POW2 + pos2] = dis;
        }
    }
}

}
