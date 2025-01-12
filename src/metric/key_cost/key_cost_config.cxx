#include "key_cost_config.hxx"

namespace clubmoss::metric::key_cost {

auto Config::getInstance() -> Config& {
    static Config instance;
    return instance;
}

auto Config::loadCfg(const Toml& cfg) -> void {
    loadHitEfforts(cfg);
    loadFinLimits(cfg);
    loadHandLimit(cfg);
}

auto Config::loadHitEfforts(const Toml& cfg) -> void {
    const Toml& node = cfg.at("hit_efforts");
    // [击键消耗]数组的长度必须等于按键的数量
    if (node.size() != KEY_COUNT) {
        throw IllegalCfg(
            "illegal size of `hit_efforts`",
            node, std::format("should be {}", KEY_COUNT)
        );
    }
    // [击键消耗]数组的元素必须是整数，且在 [0, 10] 之间
    for (const auto& [i, elem] : node.as_array() | std::views::enumerate) {
        if (const int64_t effort = elem.as_integer();
            effort < 0 or effort > 10) {
            throw IllegalCfg(
                "illegal hit-effort value",
                elem, "should be in range [0, 10]"
            );
        } else {
            costs_[i] = static_cast<fz>(effort);
        }
    }
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
    // [最大手指使用率]数组的元素必须是浮点数，且在 (0, 0.25] 之间
    for (const auto& [fin,elem] : node.as_array() | std::views::enumerate) {
        if (fin == Finger::LeftThumb or fin == Finger::RightThumb) continue;
        if (const double limit = elem.as_floating();
            limit < 0.0 or limit > 0.25) {
            throw IllegalCfg(
                "illegal finger usage limit",
                elem, "should be in range (0, 0.25]"
            );
        } else {
            max_finger_usage_[fin] = static_cast<fz>(limit);
        }
    }
}

auto Config::loadHandLimit(const Toml& cfg) -> void {
    // [最大左右手偏移量]必须是一个浮点数，且在 (0, 0.1] 之间
    const Toml& node = cfg.at("max_hand_usage_imbalance");
    if (const double max_hand_usage_imbalance = node.as_floating();
        max_hand_usage_imbalance < 0.0 or max_hand_usage_imbalance > 0.1) {
        throw IllegalCfg(
            "illegal max finger usage",
            node, "should be in range (0, 0.1]"
        );
    } else {
        max_hand_usage_imbalance_ = static_cast<fz>(max_hand_usage_imbalance);
    }
}

}
