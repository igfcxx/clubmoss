#ifndef PREPROCESSOR_HXX
#define PREPROCESSOR_HXX

#include "p_pool.hxx"

namespace clubmoss {

class Preprocessor {
public:
    auto run() -> void;

    auto searchExtremes() -> void;
    auto estimateSize() -> void;

protected:
    auto saveStatus() -> void;

private:
    preprocessor::Pool pool_{};

    uz curr_pool_{0};
    uz best_pool_{0};

    uz stagnation_pools_{0};
    uz max_stagnation_pools_{5};

    static constexpr uz MAX_POOLS{50};

    fz best_loss_{};
    std::set<fz> candidates_{};

    uz best_size_{};
    std::array<fz, TASK_COUNT> min_costs_{};
    std::array<fz, TASK_COUNT> max_costs_{};

    auto minimizeCosts(MetricId metric, Language language) -> void;
    auto maximizeCosts(MetricId metric, Language language) -> void;

    auto tryPoolSize(uz pool_size) -> fz;
};

}

#endif //PREPROCESSOR_HXX
