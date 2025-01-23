#ifndef CLUBMOSS_PREPROCESSOR_POOL_HXX
#define CLUBMOSS_PREPROCESSOR_POOL_HXX

#include "../optimizer/o_pool.hxx"

namespace clubmoss::preprocessor {

class Pool : public optimizer::Pool {
public:
    Pool() = default;

    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;

    auto search4max(uz metric_idx) -> fz;
    auto search4min(uz metric_idx) -> fz;

protected:
    auto reinitAndEvaluateSamples(uz metric) noexcept -> void;
    auto updateAndEvaluateSamples(uz metric) noexcept -> void;

    auto sortSamplesDesc() -> void;
    auto sortSamplesAsc() -> void;

private:
};

}

#endif //CLUBMOSS_PREPROCESSOR_POOL_HXX
