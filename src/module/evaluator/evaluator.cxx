#include "evaluator.hxx"

namespace clubmoss {

auto Evaluator::evaluate(Sample& sample) noexcept -> void {
    if (Sample::enabled_[0]) evalZhKeyCost(sample);
    if (Sample::enabled_[1]) evalEnKeyCost(sample);
    if (Sample::enabled_[2]) evalZhDisCost(sample);
    if (Sample::enabled_[3]) evalEnDisCost(sample);
    if (Sample::enabled_[4]) evalZhSeqCost(sample);
    if (Sample::enabled_[5]) evalEnSeqCost(sample);
    sample.update();
}

auto Evaluator::evalZhKeyCost(Sample& sample) noexcept -> void {
    sample.raw_costs_[0] = kc_metric_.measure(sample, Resources::ZH_KC_DATA);
}

auto Evaluator::evalEnKeyCost(Sample& sample) noexcept -> void {
    sample.raw_costs_[1] = kc_metric_.measure(sample, Resources::EN_KC_DATA);
}

auto Evaluator::evalZhDisCost(Sample& sample) noexcept -> void {
    sample.raw_costs_[2] = dc_metric_.measure(sample, Resources::ZH_DC_DATA);
}

auto Evaluator::evalEnDisCost(Sample& sample) noexcept -> void {
    sample.raw_costs_[3] = dc_metric_.measure(sample, Resources::EN_DC_DATA);
}

auto Evaluator::evalZhSeqCost(Sample& sample) noexcept -> void {
    sample.raw_costs_[4] = sc_metric_.measure(sample, Resources::ZH_SC_DATA);
}

auto Evaluator::evalEnSeqCost(Sample& sample) noexcept -> void {
    sample.raw_costs_[5] = sc_metric_.measure(sample, Resources::EN_SC_DATA);
}

}
