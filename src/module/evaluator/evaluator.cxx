#include "evaluator.hxx"

namespace clubmoss {

auto Evaluator::evaluate(Sample& sample) -> void {
    sample.setCost(zh_kc_metric_.measure(sample), 0);
    sample.setCost(en_kc_metric_.measure(sample), 1);
    sample.setCost(zh_dc_metric_.measure(sample), 2);
    sample.setCost(en_dc_metric_.measure(sample), 3);
    sample.setCost(zh_sc_metric_.measure(sample), 4);
    sample.setCost(en_sc_metric_.measure(sample), 5);
    sample.update();
}

auto Evaluator::measureKeyCost(Sample& sample) -> void {
    sample.setCost(zh_kc_metric_.measure(sample), 0);
    sample.setCost(en_kc_metric_.measure(sample), 1);
}

auto Evaluator::measureDisCost(Sample& sample) -> void {
    sample.setCost(zh_dc_metric_.measure(sample), 2);
    sample.setCost(en_dc_metric_.measure(sample), 3);
}

auto Evaluator::measureSeqCost(Sample& sample) -> void {
    sample.setCost(zh_sc_metric_.measure(sample), 4);
    sample.setCost(en_sc_metric_.measure(sample), 5);
}


}
