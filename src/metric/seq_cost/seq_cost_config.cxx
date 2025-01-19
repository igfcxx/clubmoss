#include "seq_cost_config.hxx"

namespace clubmoss::metric::seq_cost {

auto Config::getInstance() -> Config& {
    static Config instance;
    return instance;
}

auto Config::loadCfg(const Toml& cfg) -> void {
    loadPainLevels(cfg);
    loadCorrCosts(cfg);
}

auto idxOf = [](const Pos pos1, const Pos pos2) -> uz {
    return pos1 * KEY_CNT_POW2 + pos2;
};


auto Config::painLvlOf(const Bigram& bigram, const Layout& layout) const -> uz {
    const Pos pos1 = layout.getPos(bigram.caps[0]);
    const Pos pos2 = layout.getPos(bigram.caps[1]);
    return pain_levels_[idxOf(pos1, pos2)];
}

auto Config::painLvlOf(const Trigram& trigram, const Layout& layout) const -> uz {
    const Pos pos1 = layout.getPos(trigram.caps[0]);
    const Pos pos2 = layout.getPos(trigram.caps[1]);
    const Pos pos3 = layout.getPos(trigram.caps[2]);
    const uz lvl1 = pain_levels_[idxOf(pos1, pos2)];
    const uz lvl2 = pain_levels_[idxOf(pos2, pos3)];
    return std::max(lvl1, lvl2);
}

auto Config::costOf(const Bigram& bigram, const Layout& layout) const -> fz {
    const uz lvl = painLvlOf(bigram, layout);
    return cost_of_pain_level_[lvl];
}

auto Config::costOf(const Trigram& trigram, const Layout& layout) const -> fz {
    const uz lvl = painLvlOf(trigram, layout);
    return cost_of_pain_level_[lvl];
}

auto Config::loadPainLevels(const Toml& cfg) -> void {
    const auto QWERTY = layout::baselines::QWERTY;
    for (const Pos src_pos : POS_SET) {
        for (const Pos dst_pos : POS_SET) {
            const Toml& node = cfg.at("pain_levels").at(src_pos).at(dst_pos);
            if (const int64_t v = node.as_integer();
                v < 0 or v >= PainLevel::_size()) {
                throw IllegalCfg(
                    "illegal pain level",
                    node, std::format("should be in [0,{})", PainLevel::_size())
                );
            } else {
                pain_levels_[idxOf(src_pos, dst_pos)] = v;
            }
        }
    }
}

auto Config::loadCorrCosts(const Toml& cfg) -> void {
    const Toml& node = cfg.at("cost_of_pain_level");
    if (node.size() != PainLevel::_size()) {
        throw IllegalCfg(
            "illegal size of `cost_of_pain_level`",
            node, std::format("should be {}", PainLevel::_size())
        );
    }
    for (const auto& [fin,elem] : node.as_array() | std::views::enumerate) {
        if (const double cost = elem.as_floating(); cost < 0.0) {
            throw IllegalCfg(
                "illegal pain cost",
                elem, "should be non-negative"
            );
        } else {
            cost_of_pain_level_[fin] = static_cast<fz>(cost);
        }
    }
}

auto Config::loadNumNgrams(const Toml& cfg) -> void {
    // [最常用n-gram数]必须为整数，且在 [5, 30] 之间
    const Toml& tn_node = cfg.at("top_ngrams");
    if (const int64_t top_ngrams = tn_node.as_integer();
        top_ngrams < 5 or top_ngrams > 30) {
        throw IllegalCfg(
            "illegal top_ngrams",
            tn_node, "should be in range [5, 30]"
        );
    } else {
        top_ngrams_ = static_cast<uz>(top_ngrams);
    }
    // [允许的错误数]必须为整数，且在 [1, 3] 之间
    const Toml& ba_node = cfg.at("bad_allowed");
    if (const int64_t bad_allowed = ba_node.as_integer();
        bad_allowed < 1 or bad_allowed > 3) {
        throw IllegalCfg(
            "illegal bad_allowed",
            ba_node, "should be in range [1, 3]"
        );
    } else {
        bad_allowed_ = static_cast<uz>(bad_allowed);
    }
}

}
