#include "layout_config.hxx"

namespace clubmoss::layout {

auto Config::getInstance() -> Config& {
    static Config instance;
    return instance;
}

Config::Config() {
    mutable_areas_.reserve(16);
    pinned_keys_.reserve(32);
    area_ids_.reserve(16);
    loadCfg(DEFAULT_CFG);
}

auto Config::loadCfg(const Toml& cfg) -> void {
    resetCriticalMembers();
    validateConfig(cfg);
    loadPinnedKeys(cfg);
    loadGivenAreas(cfg);
    buildLastArea();
}

auto Config::resetCriticalMembers() -> void {
    mutable_areas_.clear();
    pinned_keys_.clear();
    area_ids_.clear();
    num_mutable_keys_ = 0;
    num_pinned_keys_  = 0;
    num_areas_        = 0;
    where_.fill(nullptr);
}

auto Config::observed(const uz elem) const -> bool {
    return where_[elem] != nullptr;
}

auto Config::validateConfig(const Toml& cfg) -> void {
    // 统计[固定按键]的数量, 并检验字段的合法性
    if (cfg.contains("pinned_keys")) {
        const Toml& keys = cfg.at("pinned_keys");
        for (const Toml& key : keys.as_array()) {
            validateCap(key.at("cap"));
            validatePos(key.at("pos"));
        }
        num_pinned_keys_ = keys.size();
    } else {
        num_pinned_keys_ = 0;
    }

    // 检验[可变按键]的数量, 因为过少的可变按键
    if (num_mutable_keys_ = KEY_COUNT - num_pinned_keys_; num_mutable_keys_ < MIN_MUTABLE_KEYS) {
        constexpr auto what{"too few mutable keys:\n" "require at least {:d} keys, got {:d}"};
        throw IllegalCfg(std::format(what, MIN_MUTABLE_KEYS, num_mutable_keys_));
    }

    // Check mutable areas and owned keys
    if (cfg.contains("mutable_areas")) {
        const Toml& areas = cfg.at("mutable_areas");
        for (const Toml& area : areas.as_array()) {
            validateFieldSize(area);
            validateKeyValues(area);
            validatePositions(area);
        }
        num_areas_ = areas.size();
    } else {
        num_areas_ = 0;
    }
}

auto Config::validateFieldSize(const Toml& area_cfg) -> void {
    const Toml& cap_list = area_cfg.at("cap_list");
    const Toml& pos_list = area_cfg.at("pos_list");

    auto get_size = [](const Toml& node) -> uz {
        if (node.is_array()) return node.as_array().size();
        throw IllegalCfg("illegal type of field", node, "should be an array");
    };
    const uz num_val = get_size(cap_list);
    const uz num_pos = get_size(pos_list);

    if (num_val != num_pos) {
        throw IllegalCfg(
            "cannot infer area size form unequal arrays",
            cap_list, std::format("size = {:d}", num_val),
            pos_list, std::format("size = {:d}", num_pos),
            "size of `cap_list` and `pos_list` must be equal"
        );
    }

    if (const uz size = num_val; size < 2 or size > KEY_COUNT) {
        throw IllegalCfg(
            "illegal area size",
            cap_list, std::format("size = {:d}", num_val),
            pos_list, std::format("size = {:d}", num_pos),
            "area size should be in range [2, 30]"
        );
    }
}

auto Config::validateKeyValues(const Toml& area_cfg) -> void {
    for (const Toml& settings = area_cfg.at("cap_list");
         const Toml& cap : settings.as_array()) {
        validateCap(cap);
    }
}

auto Config::validatePositions(const Toml& area_cfg) -> void {
    for (const Toml& settings = area_cfg.at("pos_list");
         const Toml& pos : settings.as_array()) {
        validatePos(pos);
    }
}

auto Config::validateCap(const Toml& cap) -> void {
    const std::string& cap_str = cap.as_string();
    // [键值]应当为长度为 1 的 Toml 字符串
    if (cap_str.length() != 1) {
        throw IllegalCfg(
            "illegal key code",
            cap, "should be a single char"
        );
    }

    const auto c = static_cast<Cap>(cap_str[0]);
    // [键值]字符串的取值应当在 CAP_SET 中
    if (not Utils::isLegalCap(c)) {
        throw IllegalCfg(
            "illegal key code",
            cap, "should be a capital letter or "
            "one of the 4 symbols: ',', '.', ';' and '/'"
        );
    }

    // [键值]应当不重不漏
    if (observed(c)) {
        throw IllegalCfg(
            "duplicate keys:",
            *where_[c], "1st", cap, "2nd"
        );
    }
    where_[c] = &cap;
}

auto Config::validatePos(const Toml& pos) -> void {
    const int64_t p = pos.as_integer();
    // [键位]的取值范围应当在 [0, 29] 内
    if (not Utils::isLegalPos(p)) {
        throw IllegalCfg(
            "position out of range",
            pos, "should be in range [0, 29]"
        );
    }

    // position should not duplicate
    if (observed(p)) {
        throw IllegalCfg(
            "duplicate keys",
            *where_[p], "1st", pos, "2nd"
        );
    }
    where_[p] = &pos;
}

auto Config::loadPinnedKeys(const Toml& cfg) -> void {
    if (not cfg.contains("pinned_keys")) { return; }
    for (const Toml& item : cfg.at("pinned_keys").as_array()) {
        const auto cap = static_cast<Cap>(item.at("cap").as_string()[0]);
        const auto pos = static_cast<Pos>(item.at("pos").as_integer());
        pinned_keys_.emplace_back(cap, pos);
    }
}

auto Config::loadGivenAreas(const Toml& cfg) -> void {
    if (not cfg.contains("mutable_areas")) { return; }
    for (const auto area_cfgs = cfg.at("mutable_areas").as_array() | std::views::enumerate;
         const auto [area_id, area_cfg] : area_cfgs) {
        mutable_areas_.emplace_back(area_cfg);
        const uz area_size = mutable_areas_.back().size_;
        area_ids_.insert(area_ids_.end(), area_size, area_id);
    }
}

auto Config::buildLastArea() -> void {
    // 统计未处理的按键数量,
    uz num_unprocessed_keys_ = num_mutable_keys_;
    for (const Area& area : mutable_areas_) {
        num_unprocessed_keys_ -= area.size_;
    }

    // 若没有未处理的按键, 构建结束
    if (num_unprocessed_keys_ == 0) { return; }
    // 否则新建一个由所有未处理的按键构成的[可变区域]
    Area area(num_unprocessed_keys_);
    for (const Cap cap : CAP_SET) {
        if (not observed(cap)) {
            area.cap_list_.emplace_back(cap);
        }
    }
    for (const Pos pos : POS_SET) {
        if (not observed(pos)) {
            area.pos_list_.emplace_back(pos);
        }
    }
    std::ranges::sort(area.cap_list_);

    // 更新有关成员变量
    mutable_areas_.emplace_back(area);
    const uz area_id = num_areas_; ++num_areas_;
    area_ids_.insert(area_ids_.end(), area.size_, area_id);
    assert(area_ids_.size() == num_mutable_keys_);
    assert(mutable_areas_.size() == num_areas_);
}

}
