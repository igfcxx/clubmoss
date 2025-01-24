#ifndef CLUBMOSS_ANALYZER_HXX
#define CLUBMOSS_ANALYZER_HXX

#include "sample.hxx"
#include "../resources.hxx"
#include "../../metric/key_cost/key_cost.hxx"
#include "../../metric/dis_cost/dis_cost.hxx"
#include "../../metric/seq_cost/seq_cost.hxx"

namespace clubmoss {

class Analyzer {
public:
    auto analyze(Sample& sample) -> std::string;

protected:
    auto analyzeZhKeyCost(Sample& sample) noexcept -> void;
    auto analyzeEnKeyCost(Sample& sample) noexcept -> void;
    auto analyzeZhDisCost(Sample& sample) noexcept -> void;
    auto analyzeEnDisCost(Sample& sample) noexcept -> void;
    auto analyzeZhSeqCost(Sample& sample) noexcept -> void;
    auto analyzeEnSeqCost(Sample& sample) noexcept -> void;

private:
    metric::KeyCost zh_kc_metric_{};
    metric::DisCost zh_dc_metric_{};
    metric::SeqCost zh_sc_metric_{};
    metric::KeyCost en_kc_metric_{};
    metric::DisCost en_dc_metric_{};
    metric::SeqCost en_sc_metric_{};
};

}

#endif //CLUBMOSS_ANALYZER_HXX
