#include "evaluator.hxx"

namespace clubmoss {

auto Evaluator::evaluate(Sample& sample) -> void {
    if (Sample::enabled_[0]) sample.raw_costs_[0] = zh_kc_metric_.measure(sample);
    if (Sample::enabled_[1]) sample.raw_costs_[1] = en_kc_metric_.measure(sample);
    if (Sample::enabled_[2]) sample.raw_costs_[2] = zh_dc_metric_.measure(sample);
    if (Sample::enabled_[3]) sample.raw_costs_[3] = en_dc_metric_.measure(sample);
    if (Sample::enabled_[4]) sample.raw_costs_[4] = zh_sc_metric_.measure(sample);
    if (Sample::enabled_[5]) sample.raw_costs_[5] = en_sc_metric_.measure(sample);
    sample.update();
}

}
