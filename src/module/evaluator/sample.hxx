#ifndef CLUBMOSS_SAMPLE_HXX
#define CLUBMOSS_SAMPLE_HXX

#include "../../layout/layout.hxx"

namespace clubmoss {

class Evaluator;

namespace metric {
    class KeyCost;
    class DisCost;
    class SeqCost;
}

class Sample : public Layout {
public:
    explicit Sample(const Layout& layout);
    explicit Sample(Layout&& layout);

    auto calcLoss() -> void;
    auto calcLossWithPenalty() -> void;

    [[nodiscard]] auto getLoss() const noexcept -> fz;
    [[nodiscard]] auto getRank() const noexcept -> uz;
    [[nodiscard]] auto getFlaws() const noexcept -> uz;

    static auto loadCfg(const Toml& score_cfg, const Toml& status) -> void;

protected:
    fz loss_{std::numeric_limits<fz>::max()};
    uz rank_{std::numeric_limits<uz>::max()};
    uz flaws_{0};

    std::array<fz, TASK_COUNT> scaled_costs_{};
    std::array<fz, TASK_COUNT> raw_costs_{};
    std::array<uz, TASK_COUNT> flaw_cnt_{};

private:
    inline static std::array<fz, TASK_COUNT> biases_{};
    inline static std::array<fz, TASK_COUNT> ranges_{};
    inline static std::array<fz, TASK_COUNT> weights_{};

    static auto fetchWeight(const Toml& node) -> fz;

    static constexpr char WHAT[]{"Illegal weight config: {:s}"};
    using IllegalCfg = IllegalToml<WHAT>;

    friend class metric::KeyCost;
    friend class metric::DisCost;
    friend class metric::SeqCost;
    friend class Evaluator;
};

}

#endif //CLUBMOSS_SAMPLE_HXX
