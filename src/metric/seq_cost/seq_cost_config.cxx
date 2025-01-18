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


auto Config::levelOf(const Bigram& bigram, const Layout& layout) const -> uz {
    const Pos pos1 = layout.getPos(bigram.caps[0]);
    const Pos pos2 = layout.getPos(bigram.caps[1]);
    return pain_levels_[idxOf(pos1, pos2)];
}

auto Config::levelOf(const Trigram& trigram, const Layout& layout) const -> uz {
    const Pos pos1 = layout.getPos(trigram.caps[0]);
    const Pos pos2 = layout.getPos(trigram.caps[1]);
    const Pos pos3 = layout.getPos(trigram.caps[2]);
    const uz lvl1  = pain_levels_[idxOf(pos1, pos2)];
    const uz lvl2  = pain_levels_[idxOf(pos2, pos3)];
    return std::max(lvl1, lvl2);
}

auto Config::costOf(const Bigram& bigram, const Layout& layout) const -> fz {
    const Pos pos1 = layout.getPos(bigram.caps[0]);
    const Pos pos2 = layout.getPos(bigram.caps[1]);
    const uz lvl   = pain_levels_[idxOf(pos1, pos2)];
    return cost_of_pain_level_[lvl];

}

auto Config::costOf(const Trigram& trigram, const Layout& layout) const -> fz {
    const Pos pos1 = layout.getPos(trigram.caps[0]);
    const Pos pos2 = layout.getPos(trigram.caps[1]);
    const Pos pos3 = layout.getPos(trigram.caps[2]);
    const uz lvl1  = pain_levels_[idxOf(pos1, pos2)];
    const uz lvl2  = pain_levels_[idxOf(pos2, pos3)];
    const uz lvl   = std::max(lvl1, lvl2);
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

}
