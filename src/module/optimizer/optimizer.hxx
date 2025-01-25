#ifndef CLUBMOSS_OPTIMIZER_HXX
#define CLUBMOSS_OPTIMIZER_HXX

#include "o_pool.hxx"

namespace clubmoss {

class Optimizer {
public:
    auto run() -> void;

private:
    optimizer::Pool pool_{};

    uz curr_pool_{0};
    uz best_pool_{0};

    uz stagnation_pools_{0};
    uz max_stagnation_pools_{20};

    static constexpr uz MAX_POOLS{50};

    fz best_loss_{};
    std::vector<Sample> best_samples_;

    auto copyBestSamples() -> void;
    auto analyzeSamples() -> void;
    auto saveResults() -> void;
};

}

#endif //CLUBMOSS_OPTIMIZER_HXX
