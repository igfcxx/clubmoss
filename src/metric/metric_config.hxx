#ifndef METRIC_CONFIG_HXX
#define METRIC_CONFIG_HXX

#include "metric.hxx"

namespace clubmoss::metric {

class KeyCost;
class DisCost;
class SeqCost;

template <uz N> requires (N <= 4)
struct Ngram final {
    std::array<Cap, N> caps{0};
    fz f{0.0};

    Ngram() = default;

    explicit Ngram(const std::pair<const std::string, const Toml&>& node)
        : f(node.second.as_floating()) {
        for (const auto& [c, cap] : std::views::zip(node.first, caps)) {
            cap = static_cast<Cap>(std::toupper(c));
        }
    }

    auto operator<=>(const Ngram& other) const noexcept -> std::weak_ordering {
        if (this->f > other.f) { return std::weak_ordering::greater; }
        if (this->f < other.f) { return std::weak_ordering::less; }
        return std::weak_ordering::equivalent;
    }
};

using Bigram = Ngram<2>;
using Trigram = Ngram<3>;

class Config {
    static constexpr uz FIN_COUNT = Finger::_size();
    static constexpr uz LVL_COUNT = PainLevel::_size();
    static constexpr uz REL_COUNT = PosRelation::_size();

public:
    Config(Config&&) = delete;
    Config(const Config&) = delete;
    Config& operator=(Config&&) = delete;
    Config& operator=(const Config&) = delete;

    static auto getInstance() -> Config&;

    auto disBetween(Pos pos1, Pos pos2) const -> fz;
    auto costOf(const Bigram& bigram, const Layout& layout) const -> uz;
    auto costOf(const Trigram& trigram, const Layout& layout) const -> uz;

    static auto loadCfg(const Toml& metric_cfg, const Toml& score_cfg) -> void;

protected:
    std::array<fz, KEY_CNT_POW2 * KEY_CNT_POW2> distance_map_{0.0};

    std::array<uz, KEY_CNT_POW2 * KEY_CNT_POW2> pain_levels_{
        4, 3, 2, 1, 1, 0, 0, 0, 0, 0, 4, 4, 3, 2, 1, 0, 0, 0, 0, 0, 4, 4, 4, 2, 2, 0, 0, 0, 0, 0, 0, 0,
        3, 4, 1, 1, 1, 0, 0, 0, 0, 0, 2, 4, 2, 1, 1, 0, 0, 0, 0, 0, 4, 4, 3, 2, 3, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 2, 1, 1, 0, 0, 0, 0, 0, 2, 3, 4, 0, 2, 0, 0, 0, 0, 0, 4, 4, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 2, 3, 0, 0, 0, 0, 0, 2, 2, 4, 3, 3, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 2, 4, 3, 0, 0, 0, 0, 0, 2, 2, 3, 4, 4, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 4, 4, 3, 2, 1, 0, 0, 0, 0, 0, 4, 4, 3, 2, 2, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0,
        0, 0, 0, 0, 0, 4, 2, 1, 1, 1, 0, 0, 0, 0, 0, 4, 3, 2, 2, 2, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0,
        0, 0, 0, 0, 0, 3, 1, 2, 1, 2, 0, 0, 0, 0, 0, 1, 1, 4, 2, 2, 0, 0, 0, 0, 0, 3, 4, 4, 4, 4, 0, 0,
        0, 0, 0, 0, 0, 2, 2, 1, 2, 3, 0, 0, 0, 0, 0, 1, 1, 3, 4, 3, 0, 0, 0, 0, 0, 2, 4, 4, 4, 4, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 2, 2, 4, 0, 0, 0, 0, 0, 2, 2, 3, 4, 4, 0, 0, 0, 0, 0, 3, 4, 4, 4, 4, 0, 0,
        4, 2, 2, 2, 2, 0, 0, 0, 0, 0, 4, 2, 2, 1, 1, 0, 0, 0, 0, 0, 4, 4, 3, 1, 2, 0, 0, 0, 0, 0, 0, 0,
        4, 4, 2, 2, 2, 0, 0, 0, 0, 0, 2, 4, 0, 1, 1, 0, 0, 0, 0, 0, 4, 4, 4, 2, 2, 0, 0, 0, 0, 0, 0, 0,
        3, 2, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 4, 4, 2, 3, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 3, 3, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 4, 4, 0, 0, 0, 0, 0, 1, 1, 2, 4, 3, 0, 0, 0, 0, 0, 2, 3, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 4, 4, 2, 2, 2, 0, 0, 0, 0, 0, 3, 4, 2, 1, 1, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0,
        0, 0, 0, 0, 0, 4, 2, 1, 1, 1, 0, 0, 0, 0, 0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 4, 3, 2, 0, 0,
        0, 0, 0, 0, 0, 4, 2, 4, 2, 3, 0, 0, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 3, 2, 0, 0,
        0, 0, 0, 0, 0, 4, 2, 1, 4, 4, 0, 0, 0, 0, 0, 1, 1, 0, 4, 2, 0, 0, 0, 0, 0, 1, 2, 3, 4, 3, 0, 0,
        0, 0, 0, 0, 0, 4, 1, 1, 1, 4, 0, 0, 0, 0, 0, 1, 1, 1, 2, 4, 0, 0, 0, 0, 0, 1, 2, 4, 4, 4, 0, 0,
        4, 4, 3, 3, 3, 0, 0, 0, 0, 0, 4, 4, 3, 2, 2, 0, 0, 0, 0, 0, 4, 4, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
        4, 4, 4, 4, 3, 0, 0, 0, 0, 0, 4, 4, 4, 3, 2, 0, 0, 0, 0, 0, 4, 4, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0,
        4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 3, 4, 4, 3, 3, 0, 0, 0, 0, 0, 3, 2, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0,
        3, 3, 3, 4, 4, 0, 0, 0, 0, 0, 1, 1, 1, 4, 4, 0, 0, 0, 0, 0, 2, 2, 2, 4, 4, 0, 0, 0, 0, 0, 0, 0,
        4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 1, 2, 3, 4, 4, 0, 0, 0, 0, 0, 2, 2, 3, 4, 4, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 4, 2, 1, 1, 0, 0, 0, 0, 0, 4, 3, 3, 2, 3, 0, 0,
        0, 0, 0, 0, 0, 4, 4, 4, 3, 4, 0, 0, 0, 0, 0, 4, 4, 1, 1, 1, 0, 0, 0, 0, 0, 4, 3, 1, 1, 2, 0, 0,
        0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 2, 4, 3, 3, 0, 0, 0, 0, 0, 2, 1, 3, 1, 2, 0, 0,
        0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 2, 2, 4, 4, 0, 0, 0, 0, 0, 2, 1, 1, 4, 2, 0, 0,
        0, 0, 0, 0, 0, 3, 3, 3, 4, 4, 0, 0, 0, 0, 0, 2, 2, 2, 2, 4, 0, 0, 0, 0, 0, 1, 1, 2, 3, 4, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    std::array<fz, KEY_COUNT> key_costs_{
        9.0, 5.0, 3.0, 6.0, 7.0, 9.0, 6.0, 3.0, 5.0, 9.0,
        1.0, 0.0, 0.0, 0.0, 5.0, 5.0, 0.0, 0.0, 0.0, 1.0,
        8.0, 8.0, 6.0, 4.0, 9.0, 6.0, 4.0, 6.0, 8.0, 8.0
    };

    std::array<fz, REL_COUNT> similarity_score_{
        1.0, 0.8, 0.3
    };

    std::array<fz, FIN_COUNT> max_finger_use_{0.12, 0.16, 0.20, 0.22, 1.05, 1.05, 0.22, 0.20, 0.16, 0.12};
    std::array<fz, FIN_COUNT> max_finger_mov_{0.10, 0.16, 0.25, 0.30, 1.05, 1.05, 0.30, 0.25, 0.16, 0.10};

    fz max_hand_use_imbalance_{0.025};
    fz max_hand_mov_imbalance_{0.055};

    uz ngrams_to_test_{10};
    uz max_pain_lvl_cost_{4};
    uz max_bad_ngram_count_{1};

    auto updatePainLevels() -> void;
    auto calcDistance() -> void;

    Config();

private:
    std::array<fz, KEY_COUNT> x_{
        0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00,
        0.25, 1.25, 2.25, 3.25, 4.25, 5.25, 6.25, 7.25, 8.25, 9.25,
        0.75, 1.75, 2.75, 3.75, 4.75, 5.75, 6.75, 7.75, 8.75, 9.75,
    };

    std::array<fz, KEY_COUNT> y_{
        0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
        1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
        2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00,
    };

    std::array<uz, LVL_COUNT> cost_of_pain_level_{0, 1, 2, 4, 8};

    auto loadFingerHandLimits(const Toml& cfg) -> void;
    auto loadPainLevelLimits(const Toml& cfg) -> void;
    auto loadKeyCostCfgs(const Toml& cfg) -> void;
    auto loadDisCostCfgs(const Toml& cfg) -> void;
    auto loadSeqCostCfgs(const Toml& cfg) -> void;

    static constexpr char WHAT[]{"Illegal metric config: {:s}"};
    using IllegalCfg = IllegalToml<WHAT>;

    static auto checkArraySize(const Toml& node, std::string_view msg, size_t expected_size) -> void;
    static auto fetchFloat(const Toml& node, std::string_view msg, fz min, fz max) -> fz;
    static auto fetchInt(const Toml& node, std::string_view msg, uz min, uz max) -> uz;

    friend class KeyCost;
    friend class DisCost;
    friend class SeqCost;
};

}

#endif // METRIC_CONFIG_HXX
