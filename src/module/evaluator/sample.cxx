#include "sample.hxx"

namespace clubmoss {

Sample::Sample(const Layout& layout) : Layout(layout) {}
Sample::Sample(Layout&& layout) : Layout(layout) {}

auto Sample::update() -> void {
    for (uz i = 0; i < METRICS; ++i) {
        const fz cost = (raw_costs_[i] - biases_[i]) / ranges_[i];
        scaled_costs_[i] = std::clamp(cost, 0.0, 1.0);
    }

    loss_ = 0.0;
    for (uz i = 0; i < METRICS; ++i) {
        if (use_ols_) {
            loss_ += scaled_costs_[i] * scaled_costs_[i] * weights_[i];
        } else {
            loss_ += scaled_costs_[i] * weights_[i];
        }
    }
}

auto Sample::getLoss() const -> fz {
    return loss_;
}

auto Sample::setCost(const fz cost, const uz index) -> void {
    raw_costs_[index] = cost;
}

auto Sample::enabled(const uz index) -> bool {
    return enabled_[index];
}

auto Sample::loadWeights(const Toml& cfg) -> void {
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

    use_ols_ = cfg.at("use_ols").as_boolean();

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
    const auto b_table = cfg.at("biases").as_array();
    const auto r_table = cfg.at("ranges").as_array();
    for (uz i = 0; i < METRICS; ++i) {
        biases_[i] = b_table.at(i).as_floating();
        ranges_[i] = r_table.at(i).as_floating();
    }
}

}
