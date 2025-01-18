#ifndef CLUBMOSS_KEY_COST_CONFIG_HXX
#define CLUBMOSS_KEY_COST_CONFIG_HXX

#include "key_cost_data.hxx"

namespace clubmoss::metric::key_cost {
class Config final {
public:
    Config(Config&&)                 = delete;
    Config(const Config&)            = delete;
    Config& operator=(Config&&)      = delete;
    Config& operator=(const Config&) = delete;

    static auto getInstance() -> Config&;

    auto loadCfg(const Toml& cfg) -> void;

protected:
    // 击键代价
    std::array<fz, KEY_COUNT> costs_{
        9.0, 5.0, 3.0, 6.0, 7.0, 9.0, 6.0, 3.0, 5.0, 9.0,
        1.0, 0.0, 0.0, 0.0, 5.0, 5.0, 0.0, 0.0, 0.0, 1.0,
        8.0, 8.0, 6.0, 4.0, 9.0, 6.0, 4.0, 6.0, 8.0, 8.0
    };

    // 手指使用率平衡性限制
    std::array<fz, Finger::_size()> max_finger_usage_{
        0.12, 0.16, 0.20, 0.22, 1.05, 1.05, 0.22, 0.20, 0.16, 0.12
    };
    fz max_hand_usage_imbalance_{0.025};

    std::array<fz, PosRelation::_size()> similarity_score_{
        1.0, 0.8, 0.3, 0.0
    };

    Config() = default;

    auto loadHitEfforts(const Toml& cfg) -> void;
    auto loadFinLimits(const Toml& cfg) -> void;
    auto loadHandLimit(const Toml& cfg) -> void;
    auto loadSimScore(const Toml& cfg) -> void;

private:
    static constexpr char WHAT[]{"Illegal key-cost config: {:s}"};
    using IllegalCfg = IllegalToml<WHAT>;

    friend class clubmoss::metric::KeyCost;
};

}

#endif // CLUBMOSS_KEY_COST_CONFIG_HXX
