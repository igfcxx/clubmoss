#include "evaluator.hxx"

namespace clubmoss {
Evaluator::Evaluator() {
    initMetrics();
}

Evaluator::Evaluator(const Evaluator&) {
    if (metrics_.empty()) {
        initMetrics();
    }
}

Evaluator& Evaluator::operator=(const Evaluator&) {
    if (metrics_.empty()) {
        initMetrics();
    }
    return *this;
}

auto Evaluator::initMetrics() -> void {
    using R = Resources;
    for (const MetricId metric : MetricId::_values()) {
        for (const Language lang : Language::_values()) {
            switch (metric._value) {
            case MetricId::KeyCost:
                metrics_.emplace_back(metric::KeyCost(R::KC_DATA[lang]));
                break;
            case MetricId::DisCost:
                metrics_.emplace_back(metric::DisCost(R::DC_DATA[lang]));
                break;
            case MetricId::SeqCost:
                metrics_.emplace_back(metric::SeqCost(R::SC_DATA[lang]));
                break;
            default:
                throw FatalError("Invalid metric id");
            }
        }
    }
}

auto Evaluator::loadEnabledFlags() -> void {
    for (const MetricId metric : MetricId::_values()) {
        for (const Language lang : Language::_values()) {
            const uz i = Utils::taskIdOf(metric, lang);
            enabled_[i] = (Sample::weights_[i] != 0.0);
        }
    }
}

auto Evaluator::measure(Sample& sample) const noexcept -> void {
    for (uz i = 0; i < metrics_.size(); ++i) {
        sample.raw_costs_[i] = enabled_[i] ? metrics_[i].measure(sample) : 0.0;
    }
    sample.calcLoss();
}

auto Evaluator::analyze(Sample& sample) const noexcept -> void {
    for (uz i = 0; i < metrics_.size(); ++i) {
        if (enabled_[i]) {
            const auto pair = metrics_[i].analyze(sample);
            sample.raw_costs_[i] = pair.first;
            sample.flaw_cnt_[i] = pair.second;
        } else {
            sample.raw_costs_[i] = 0.0;
            sample.flaw_cnt_[i] = 0;
        }
    }
    sample.calcLossWithPenalty();
}

auto Evaluator::evaluate(Sample& sample) const noexcept -> std::string {
    const toml::ordered_table lang_stats{
        {"heat_map", toml::array{}},
        {"row_usage", toml::array{}},
        {"col_usage", toml::array{}},
        {"l_hand_usage", toml::ordered_table{}},
        {"r_hand_usage", toml::ordered_table{}},
        {"is_finger_hit_exceeded", toml::array{}},
        {"is_hand_hit_unbalanced", toml::ordered_table{}},
        {"finger_move", toml::array{}},
        {"l_hand_move", toml::ordered_table{}},
        {"r_hand_move", toml::ordered_table{}},
        {"is_finger_dis_exceeded", toml::array{}},
        {"is_hand_dis_unbalanced", toml::ordered_table{}},
        {"2_gram_pain_levels", toml::array{}},
        {"3_gram_pain_levels", toml::array{}},
        {"similarity", toml::ordered_table{}},
    };

    Toml zh_stats(lang_stats), en_stats(lang_stats);

    for (const MetricId metric : MetricId::_values()) {
        for (const Language lang : Language::_values()) {
            const uz i = Utils::taskIdOf(metric, lang);
            if (lang._value == Language::Chinese) {
                const auto pair = metrics_[i].scan(sample, zh_stats);
                sample.raw_costs_[i] = pair.first;
                sample.flaw_cnt_[i] = pair.second;
            } else if (lang._value == Language::English) {
                const auto pair = metrics_[i].scan(sample, en_stats);
                sample.raw_costs_[i] = pair.first;
                sample.flaw_cnt_[i] = pair.second;
            }
        }
    }
    sample.calcLossWithPenalty();

    const Toml stats(
        toml::ordered_table{
            {"keys", sample.toString()},
            {"loss", sample.loss_},
            {"flaws", sample.flaws_},
            {"costs", sample.scaled_costs_},
            {"raw_costs", sample.raw_costs_},
            {"flaw_cnt", sample.flaw_cnt_},
            {"weights", Sample::weights_},
            {"enabled", enabled_},
            {"zh_stats", zh_stats},
            {"en_stats", en_stats},
        }
    );
    return toml::format(stats);
}

auto Evaluator::measure(Sample& sample, const uz task_id) const noexcept -> void {
    sample.loss_ = metrics_[task_id].measure(sample);
}

}
