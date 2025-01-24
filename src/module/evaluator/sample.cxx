#include "sample.hxx"

namespace clubmoss {

Sample::Sample(const Layout& layout) : Layout(layout) {}

Sample::Sample(Layout&& layout) : Layout(layout) {}

auto Sample::calcLoss() -> void {
    for (uz i = 0; i < TASK_COUNT; ++i) {
        const fz cost = (raw_costs_[i] - biases_[i]) / ranges_[i];
        scaled_costs_[i] = std::clamp(cost, 0.0, 1.0);
    }

    loss_ = 0.0;
    for (uz i = 0; i < TASK_COUNT; ++i) {
        if (use_ols_) {
            loss_ += scaled_costs_[i] * scaled_costs_[i] * weights_[i];
        } else {
            loss_ += scaled_costs_[i] * weights_[i];
        }
    }
}

auto Sample::calcLossWithPenalty() -> void {
    calcLoss();

    for (uz i = 0; i < TASK_COUNT; ++i) {
        if (not valid_[i]) {
            loss_ += 0.1;
        }
    }
}

auto Sample::getLoss() const noexcept -> fz {
    return loss_;
}

auto Sample::loadWeights(const Toml& cfg) -> void {
    use_ols_ = cfg.at("use_ols").as_boolean();

    weights_.fill(0.0);
    for (const Language lang : Language::_values()) {
        for (const MetricId metric : MetricId::_values()) {
            const std::string lang_name = Utils::toSnakeCase(lang._to_string());
            const std::string metric_name = Utils::toSnakeCase(metric._to_string());

            const fz w = fetchWeight(cfg.at(lang_name).at(metric_name));
            const uz i = metric * Language::_size() + lang;
            enabled_[i] = w != 0.0;
            weights_[i] = w;
        }
    }

    const fz total_weight = Utils::sum(weights_);
    for (fz& weight : weights_) {
        weight /= total_weight;
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

auto Sample::loadFactors(const Toml& cfg) -> void {
    for (uz task_id = 0; task_id < TASK_COUNT; ++task_id) {
        biases_[task_id] = cfg.at("biases").as_array().at(task_id).as_floating();
        ranges_[task_id] = cfg.at("ranges").as_array().at(task_id).as_floating();
    }
}

}
