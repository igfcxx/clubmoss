#ifndef CLUBMOSS_EVALUATOR_HXX
#define CLUBMOSS_EVALUATOR_HXX

#include "sample.hxx"
#include "../../common/resources.hxx"
#include "../../metric/key_cost/key_cost.hxx"
#include "../../metric/dis_cost/dis_cost.hxx"
#include "../../metric/seq_cost/seq_cost.hxx"

namespace clubmoss {

class Evaluator {
public:
    Evaluator();

    auto evaluateFast(Sample& sample) const noexcept -> void;
    auto evaluateFull(Sample& sample) const noexcept -> void;

private:
    std::vector<Metric> metrics_;
};

}

#endif //CLUBMOSS_EVALUATOR_HXX
