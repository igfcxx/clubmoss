#include "metric_config.hxx"

namespace clubmoss::metric {

Config::Config() {
    calcNgramCosts();
    calcDistance();
}

auto Config::getInstance() -> Config& {
    static Config instance;
    return instance;
}

auto index = [](const Pos pos1, const Pos pos2) -> uz {
    return pos1 * KEY_CNT_POW2 + pos2;
};

auto Config::disBetween(const Pos pos1, const Pos pos2) const -> fz {
    return distance_map_[index(pos1, pos2)];
}

auto Config::costOf(const Bigram& bigram, const Layout& layout) const -> uz {
    const Cap cap1 = bigram.caps[0];
    const Cap cap2 = bigram.caps[1];
    const Pos pos1 = layout.getPos(cap1);
    const Pos pos2 = layout.getPos(cap2);
    return ngram_costs_[index(pos1, pos2)];
}

auto Config::costOf(const Trigram& trigram, const Layout& layout) const -> uz {
    const Cap cap1 = trigram.caps[0];
    const Cap cap2 = trigram.caps[1];
    const Cap cap3 = trigram.caps[2];
    const Pos pos1 = layout.getPos(cap1);
    const Pos pos2 = layout.getPos(cap2);
    const Pos pos3 = layout.getPos(cap3);
    const uz lvl1 = ngram_costs_[index(pos1, pos2)];
    const uz lvl2 = ngram_costs_[index(pos2, pos3)];
    return std::max(lvl1, lvl2);
}

auto Config::painLevelOf(const Bigram& bigram, const Layout& layout) const -> uz {
    const Cap cap1 = bigram.caps[0];
    const Cap cap2 = bigram.caps[1];
    const Pos pos1 = layout.getPos(cap1);
    const Pos pos2 = layout.getPos(cap2);
    return pain_levels_[index(pos1, pos2)];
}

auto Config::painLevelOf(const Trigram& trigram, const Layout& layout) const -> uz {
    const Cap cap1 = trigram.caps[0];
    const Cap cap2 = trigram.caps[1];
    const Cap cap3 = trigram.caps[2];
    const Pos pos1 = layout.getPos(cap1);
    const Pos pos2 = layout.getPos(cap2);
    const Pos pos3 = layout.getPos(cap3);
    const uz lvl1 = pain_levels_[index(pos1, pos2)];
    const uz lvl2 = pain_levels_[index(pos2, pos3)];
    return std::max(lvl1, lvl2);
}

auto Config::loadCfg(const Toml& metric_cfg, const Toml& score_cfg) -> void {
    Config& instance = getInstance();
    instance.loadFingerHandLimits(score_cfg.at("limits"));
    instance.loadPainLevelLimits(score_cfg.at("limits"));
    instance.loadKeyCostCfgs(metric_cfg.at("key_cost"));
    instance.loadDisCostCfgs(metric_cfg.at("dis_cost"));
    instance.loadSeqCostCfgs(metric_cfg.at("seq_cost"));
    instance.calcNgramCosts();
    instance.calcDistance();
}

auto Config::loadFingerHandLimits(const Toml& cfg) -> void {
    const Toml& mfu_node = cfg.at("max_finger_use");
    checkArraySize(mfu_node, "max_finger_use", FIN_COUNT);
    for (const auto& [fin,elem] : mfu_node.as_array() | std::views::enumerate) {
        if (fin == Finger::LeftThumb or fin == Finger::RightThumb) { continue; }
        max_finger_use_[fin] = fetchFloat(elem, "max_finger_use", 0.05, 0.25);
    }

    const Toml& mfm_node = cfg.at("max_finger_mov");
    checkArraySize(mfm_node, "max_finger_mov", FIN_COUNT);
    for (const auto& [fin,elem] : mfm_node.as_array() | std::views::enumerate) {
        if (fin == Finger::LeftThumb or fin == Finger::RightThumb) { continue; }
        max_finger_mov_[fin] = fetchFloat(elem, "max_finger_mov", 0.05, 0.35);
    }

    const Toml& mhu_node = cfg.at("max_hand_use_imbalance");
    max_hand_use_imbalance_ = fetchFloat(mhu_node, "max_hand_use_imbalance", 0.01, 0.10);

    const Toml& mhm_node = cfg.at("max_hand_mov_imbalance");
    max_hand_mov_imbalance_ = fetchFloat(mhm_node, "max_hand_mov_imbalance", 0.01, 0.10);
}

auto Config::loadPainLevelLimits(const Toml& cfg) -> void {
    const Toml& tk_node = cfg.at("test_top_k_ngrams");
    ngrams_to_test_ = fetchInt(tk_node, "test_top_k_ngrams", 3, 25);

    const Toml& apl_node = cfg.at("allow_pain_level");
    const uz allow_pain_level = fetchInt(apl_node, "allow_pain_level", 2, 4);
    max_ngram_cost_ = cost_of_pain_level_[allow_pain_level];
}

auto Config::loadKeyCostCfgs(const Toml& cfg) -> void {
    const Toml& kc_node = cfg.at("key_costs");
    checkArraySize(kc_node, "key_costs", KEY_COUNT);
    for (const auto& [i, elem] : kc_node.as_array() | std::views::enumerate) {
        key_costs_[i] = fetchInt(elem, "key cost", 0, 10);
    }

    const Toml& sc_node = cfg.at("similarity_scores");
    checkArraySize(sc_node, "similarity_scores", REL_COUNT);
    for (const auto& [i,elem] : sc_node.as_array() | std::views::enumerate) {
        similarity_score_[i] = fetchFloat(elem, "similarity score", 0.0, 1.0);
    }
}

auto Config::loadDisCostCfgs(const Toml& cfg) -> void {
    const Toml& cd_node = cfg.at("coordinates");
    checkArraySize(cd_node, "coordinates", KEY_COUNT);
    for (const auto& [i, elem] : cd_node.as_array() | std::views::enumerate) {
        checkArraySize(elem, "coordinate", 2);
        x_[i] = fetchFloat(elem.at(0), "x-coordinate", 0.0, 30.0);
        y_[i] = fetchFloat(elem.at(1), "y-coordinate", 0.0, 30.0);
    }
}

auto Config::loadSeqCostCfgs(const Toml& cfg) -> void {
    const Toml& pl_node = cfg.at("pain_levels");
    checkArraySize(pl_node, "pain_levels", KEY_COUNT);
    for (const Pos src_pos : POS_SET) {
        const Toml& line = pl_node.at(src_pos);
        checkArraySize(line, "pain_levels", KEY_COUNT);
        for (const Pos dst_pos : POS_SET) {
            const Toml& node = line.at(dst_pos);
            pain_levels_[index(src_pos, dst_pos)] = fetchInt(
                node, "pain level", 0, LVL_COUNT - 1
            );
        }
    }

    const Toml& cp_node = cfg.at("cost_of_pain_levels");
    checkArraySize(cp_node, "pain level cost", LVL_COUNT);
    for (const auto& [i,elem] : cp_node.as_array() | std::views::enumerate) {
        cost_of_pain_level_[i] = fetchInt(elem, "pain level cost", 0, 10);
    }
}

auto Config::calcNgramCosts() -> void {
    for (const Pos pos1 : POS_SET) {
        for (const Pos pos2 : POS_SET) {
            const uz idx = index(pos1, pos2);
            const uz lvl = pain_levels_[idx];
            ngram_costs_[idx] = cost_of_pain_level_[lvl];
        }
    }
}

auto Config::calcDistance() -> void {
    for (const Pos pos1 : POS_SET) {
        for (const Pos pos2 : POS_SET) {
            const fz x1 = x_[pos1], y1 = y_[pos1];
            const fz x2 = x_[pos2], y2 = y_[pos2];
            const fz dis = std::hypot(x1 - x2, y1 - y2);
            distance_map_[pos1 * KEY_CNT_POW2 + pos2] = dis;
        }
    }
}

auto Config::checkArraySize(const Toml& node, const std::string_view msg, const size_t expected_size) -> void {
    if (node.size() != expected_size) {
        throw IllegalCfg(
            std::format("illegal size of `{:s}`", msg),
            node, std::format("should be {:d}", expected_size)
        );
    }
}

auto Config::fetchFloat(const Toml& node, const std::string_view msg, const fz min, const fz max) -> fz {
    const double value = node.as_floating();
    if (value < min or value > max) {
        throw IllegalCfg(
            std::format("illegal `{:s}` value", msg),
            node, std::format("should be in range [{:f}, {:f}]", min, max)
        );
    }
    return static_cast<fz>(value);
}

auto Config::fetchInt(const Toml& node, const std::string_view msg, const uz min, const uz max) -> uz {
    const double value = node.as_integer();
    if (value < min or value > max) {
        throw IllegalCfg(
            std::format("illegal `{:s}` value", msg),
            node, std::format("should be in range [{:d}, {:d}]", min, max)
        );
    }
    return static_cast<uz>(value);
}

}
