#ifndef CLUBMOSS_SAMPLE_HXX
#define CLUBMOSS_SAMPLE_HXX

#include "../../layout/layout.hxx"

namespace clubmoss {

class Evaluator;
class Analyzer;

class Sample : public Layout {
public:
    explicit Sample(const Layout& layout);
    explicit Sample(Layout&& layout);

    auto calcLoss() -> void;
    auto calcLossWithPenalty() -> void;

    [[nodiscard]] auto getLoss() const noexcept -> fz;

    static auto loadCfg(const Toml& score_cfg, const Toml& status) -> void;

protected:
    fz loss_{std::numeric_limits<fz>::max()};

    std::array<fz, TASK_COUNT> scaled_costs_{};
    std::array<fz, TASK_COUNT> raw_costs_{};
    std::array<bool, TASK_COUNT> valid_{};

private:
    inline static std::array<fz, TASK_COUNT> biases_{};
    inline static std::array<fz, TASK_COUNT> ranges_{};

    inline static std::array<bool, TASK_COUNT> enabled_{};
    inline static std::array<fz, TASK_COUNT> weights_{};
    inline static bool use_ols_ = true;

    static auto fetchWeight(const Toml& node) -> fz;

    static constexpr char WHAT[]{"Illegal weight config: {:s}"};
    using IllegalCfg = IllegalToml<WHAT>;

    friend class Evaluator;
    friend class Analyzer;
};

}

#endif //CLUBMOSS_SAMPLE_HXX
