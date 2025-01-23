#ifndef CLUBMOSS_OPTIMIZER_POOL_HXX
#define CLUBMOSS_OPTIMIZER_POOL_HXX

#include "../evaluator/evaluator.hxx"

namespace clubmoss::optimizer {

class Pool {
public:
    Pool();

    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;

    auto search() noexcept -> void;

    auto setSize(uz size) noexcept -> void;

protected:
    std::vector<std::unique_ptr<Sample>> samples_{};
    layout::Manager mgr_{};
    Evaluator evl_{};

    uz size_{9600};
    uz half_{4800};

    fz best_loss_{-1};

    uz curr_epoch_{0};
    uz best_epoch_{0};

    uz stagnation_epochs_{0};
    uz max_stagnation_epochs_{250};

    static constexpr uz MAX_EPOCHS{1000};

    auto reinitAndEvaluateSamples() noexcept -> void;
    auto updateAndEvaluateSamples() noexcept -> void;
    auto sortSamples() -> void;
};

}

#endif //CLUBMOSS_OPTIMIZER_POOL_HXX
