#ifndef CLUBMOSS_DIS_COST_CONFIG_HXX
#define CLUBMOSS_DIS_COST_CONFIG_HXX

#include "dis_cost_data.hxx"

namespace clubmoss::metric::dis_cost {
class Config final {
public:
    Config(Config&&)                 = delete;
    Config(const Config&)            = delete;
    Config& operator=(Config&&)      = delete;
    Config& operator=(const Config&) = delete;

    static auto getInstance() -> Config&;

    auto loadCfg(const Toml& cfg) -> void;

    auto disBetween(Pos pos1, Pos pos2) const -> fz;

protected:
    std::array<fz, KEY_CNT_POW2 * KEY_CNT_POW2> dis_;
    std::array<uz, Finger::_size()> base_pos_{
        10, 11, 12, 13, 30, 30, 16, 17, 18, 19
    };

    // 手指使用率平衡性限制
    std::array<fz, Finger::_size()> max_finger_usage_{
        0.10, 0.16, 0.25, 0.30, 1.05, 1.05, 0.30, 0.25, 0.16, 0.10
    };
    fz max_hand_usage_imbalance_{0.055};

    Config() = default;

    auto loadFinLimits(const Toml& cfg) -> void;
    auto loadHandLimit(const Toml& cfg) -> void;
    auto clacDistance() -> void;

private:
    std::array<fz, KEY_COUNT> x_ {
        0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00,
        0.25, 1.25, 2.25, 3.25, 4.25, 5.25, 6.25, 7.25, 8.25, 9.25,
        0.75, 1.75, 2.75, 3.75, 4.75, 5.75, 6.75, 7.75, 8.75, 9.75,
    };
    std::array<fz, KEY_COUNT> y_{
        0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
        1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
        2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00,
    };

    static constexpr char WHAT[]{"Illegal dis-cost config: {:s}"};
    using IllegalCfg = IllegalToml<WHAT>;

    friend class clubmoss::metric::DisCost;
};

}

#endif // CLUBMOSS_DIS_COST_CONFIG_HXX
