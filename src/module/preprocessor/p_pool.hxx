#ifndef CLUBMOSS_PREPROCESSOR_POOL_HXX
#define CLUBMOSS_PREPROCESSOR_POOL_HXX

#include "../optimizer/o_pool.hxx"

namespace clubmoss {
class Preprocessor;
}

namespace clubmoss::preprocessor {

class Pool : public optimizer::Pool {
public:
    Pool() = default;

    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;

    auto search4min(uz task_id) -> fz;
    auto search4max(uz task_id) -> fz;

protected:
    auto reinitAndEvaluateSamples(uz task_id) noexcept -> void;
    auto updateAndEvaluateSamples(uz task_id) noexcept -> void;

    auto sortSamplesDesc() -> void;
    auto sortSamplesAsc() -> void;

private:
    friend class clubmoss::Preprocessor;
};

}

#endif //CLUBMOSS_PREPROCESSOR_POOL_HXX
