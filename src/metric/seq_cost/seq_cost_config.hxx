#ifndef CLUBMOSS_SEQ_COST_CONFIG_HXX
#define CLUBMOSS_SEQ_COST_CONFIG_HXX

#include "seq_cost_data.hxx"

namespace clubmoss::metric::seq_cost {
class Config final {
public:
    Config(Config&&) = delete;
    Config(const Config&) = delete;
    Config& operator=(Config&&) = delete;
    Config& operator=(const Config&) = delete;

    static auto getInstance() -> Config&;

    auto loadCfg(const Toml& cfg) -> void;

    [[nodiscard]] auto painLvlOf(const Bigram& bigram, const Layout& layout) const -> uz;
    [[nodiscard]] auto painLvlOf(const Trigram& trigram, const Layout& layout) const -> uz;

    [[nodiscard]] auto costOf(const Bigram& bigram, const Layout& layout) const -> fz;
    [[nodiscard]] auto costOf(const Trigram& trigram, const Layout& layout) const -> fz;

protected:
    // 不适程度
    std::array<uz, KEY_CNT_POW2 * KEY_CNT_POW2> pain_levels_{0};

    // 不适程度对应的代价分数
    std::array<fz, PainLevel::_size()> cost_of_pain_level_{
        0.0, 1.0, 2.0, 4.0, 8.0
    };

    // 前 N 个 N-gram 的不适程度不应过大
    uz top_ngrams_{10};
    uz bad_allowed_{1};

    Config() = default;

    auto loadPainLevels(const Toml& cfg) -> void;
    auto loadCorrCosts(const Toml& cfg) -> void;
    auto loadNumNgrams(const Toml& cfg) -> void;

private:
    static constexpr char WHAT[]{"Illegal seq-cost config: {:s}"};
    using IllegalCfg = IllegalToml<WHAT>;

    friend class clubmoss::metric::SeqCost;
};

}

#endif // CLUBMOSS_SEQ_COST_CONFIG_HXX
