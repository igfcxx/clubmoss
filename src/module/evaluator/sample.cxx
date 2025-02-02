#include "sample.hxx"

namespace clubmoss {

Sample::Sample(const Layout& layout) : Layout(layout) {}

Sample::Sample(Layout&& layout) : Layout(layout) {}

Sample::Sample(const layout::baselines::Baseline& baseline)
    : Layout(baseline), name_{baseline.name} {}

auto Sample::calcLoss() -> void {
    for (uz i = 0; i < TASK_COUNT; ++i) {
        const fz cost = (raw_costs_[i] - biases_[i]) / ranges_[i];
        scaled_costs_[i] = std::clamp(cost, 0.0, 1.0);
    }

    loss_ = 0.0;
    for (uz i = 0; i < TASK_COUNT; ++i) {
        loss_ += scaled_costs_[i] * weights_[i];
    }
}

auto Sample::calcLossWithPenalty() -> void {
    calcLoss();
    flaws_ = 0;
    for (uz i = 0; i < TASK_COUNT; ++i) {
        flaws_ += flaw_cnt_[i];
    }
    loss_ += 0.01 * flaws_;
}

auto Sample::getLoss() const noexcept -> fz {
    return loss_;
}

auto Sample::getRank() const noexcept -> uz {
    return rank_;
}

auto Sample::getFlaws() const noexcept -> uz {
    return flaws_;
}

auto Sample::loadCfg(const Toml& score_cfg, const Toml& status) -> void {
    weights_.fill(0.0);
    for (const Language lang : Language::_values()) {
        for (const MetricId metric : MetricId::_values()) {
            const std::string lang_name = Utils::toSnakeCase(lang._to_string());
            const std::string metric_name = Utils::toSnakeCase(metric._to_string());

            const fz w = fetchWeight(score_cfg.at("weights").at(lang_name).at(metric_name));
            const uz i = metric * Language::_size() + lang;
            weights_[i] = w;
        }
    }

    const fz total_weight = Utils::sum(weights_);
    for (fz& weight : weights_) {
        weight /= total_weight;
    }

    for (uz task_id = 0; task_id < TASK_COUNT; ++task_id) {
        biases_[task_id] = status.at("biases").as_array().at(task_id).as_floating();
        ranges_[task_id] = status.at("ranges").as_array().at(task_id).as_floating();
    }
}

auto Sample::fetchWeight(const Toml& node) -> fz {
    const fz weight = node.as_floating();
    if (std::abs(weight) < 1e-3) {
        return 0.0;
    }
    if (weight < 0.5 or weight > 5.0) {
        throw IllegalCfg(
            "illegal weight value",
            node, "should be in range [0.5, 5.0]"
        );
    }
    return weight;
}

}
