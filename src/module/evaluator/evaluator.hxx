#ifndef CLUBMOSS_EVALUATOR_HXX
#define CLUBMOSS_EVALUATOR_HXX

#include "sample.hxx"
#include "../resources.hxx"
#include "../../metric/key_cost/key_cost.hxx"
#include "../../metric/dis_cost/dis_cost.hxx"
#include "../../metric/seq_cost/seq_cost.hxx"

namespace clubmoss {

class Evaluator {
public:
    auto evaluate(Sample& sample) noexcept -> void;

    auto calcCost(Sample& sample, uz metric) noexcept -> void;

protected:
    auto evalZhKeyCost(Sample& sample) noexcept -> void;
    auto evalEnKeyCost(Sample& sample) noexcept -> void;
    auto evalZhDisCost(Sample& sample) noexcept -> void;
    auto evalEnDisCost(Sample& sample) noexcept -> void;
    auto evalZhSeqCost(Sample& sample) noexcept -> void;
    auto evalEnSeqCost(Sample& sample) noexcept -> void;

private:
    metric::KeyCost kc_metric_;
    metric::DisCost dc_metric_;
    metric::SeqCost sc_metric_;
};

}

#endif //CLUBMOSS_EVALUATOR_HXX
