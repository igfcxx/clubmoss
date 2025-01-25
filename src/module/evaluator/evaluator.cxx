#include "evaluator.hxx"

namespace clubmoss {
Evaluator::Evaluator() {
    using R = Resources;
    for (const MetricId metric : MetricId::_values()) {
        for (const Language lang : Language::_values()) {
            switch (metric) {
            case MetricId::KeyCost:
                metrics_.emplace_back(
                    metric::KeyCost(std::move(R::KC_DATA[lang]))
                );
                break;
            case MetricId::DisCost:
                metrics_.emplace_back(
                    metric::DisCost(std::move(R::DC_DATA[lang]))
                );
                break;
            case MetricId::SeqCost:
                metrics_.emplace_back(
                    metric::SeqCost(std::move(R::SC_DATA[lang]))
                );
                break;
            default:
                break;
            }
        }
    }
}

auto Evaluator::evaluateFast(Sample& sample) const noexcept -> void {
    for (uz i = 0; i < metrics_.size(); ++i) {
        sample.raw_costs_[i] = Sample::enabled_[i] ? metrics_[i].measure(sample) : 0.0;
    }
    sample.calcLoss();
}

auto Evaluator::evaluateFull(Sample& sample) const noexcept -> void {
    for (uz i = 0; i < metrics_.size(); ++i) {
        sample.raw_costs_[i] = Sample::enabled_[i] ? metrics_[i].analyze(sample) : 0.0;
    }
    sample.calcLossWithPenalty();
}

}
