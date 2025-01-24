#include "analyzer.hxx"

namespace clubmoss {

auto Analyzer::analyze(Sample& sample) -> std::string {
    analyzeZhKeyCost(sample);
    analyzeEnKeyCost(sample);
    analyzeZhDisCost(sample);
    analyzeEnDisCost(sample);
    analyzeZhSeqCost(sample);
    analyzeEnSeqCost(sample);

    sample.calcLossWithPenalty();

    const Toml zh_stats(
        toml::ordered_table{
            {"heat_map", zh_kc_metric_.heat_map_},
            {"row_use", zh_kc_metric_.row_usage_},
            {"col_use", zh_kc_metric_.col_usage_},
            {"l_hand_use", zh_kc_metric_.left_hand_usage_},
            {"bad_finger_use", zh_kc_metric_.is_finger_overused_},
            {"unbalanced_use", zh_kc_metric_.is_hand_unbalanced_},
            {"finger_move", zh_dc_metric_.finger_move_},
            {"l_hand_move", zh_dc_metric_.left_hand_usage_},
            {"bad_finger_move", zh_dc_metric_.is_finger_overused_},
            {"unbalanced_move", zh_dc_metric_.is_hand_unbalanced_},
            {"2gram_pain_levels", zh_sc_metric_.pain_lvl_of_top_2_grams_},
            {"3gram_pain_levels", zh_sc_metric_.pain_lvl_of_top_3_grams_},
        }
    );

    const Toml en_stats(
        toml::ordered_table{
            {"heat_map", en_kc_metric_.heat_map_},
            {"row_use", en_kc_metric_.row_usage_},
            {"col_use", en_kc_metric_.col_usage_},
            {"l_hand_use", en_kc_metric_.left_hand_usage_},
            {"bad_finger_use", en_kc_metric_.is_finger_overused_},
            {"unbalanced_use", en_kc_metric_.is_hand_unbalanced_},
            {"finger_move", en_dc_metric_.finger_move_},
            {"l_hand_move", en_dc_metric_.left_hand_usage_},
            {"bad_finger_move", en_dc_metric_.is_finger_overused_},
            {"unbalanced_move", en_dc_metric_.is_hand_unbalanced_},
            {"2gram_pain_levels", en_sc_metric_.pain_lvl_of_top_2_grams_},
            {"3gram_pain_levels", en_sc_metric_.pain_lvl_of_top_3_grams_},
        }
    );

    const fz similarity = (zh_kc_metric_.similarity_ + en_kc_metric_.similarity_) / 2;

    const Toml stats(
        toml::ordered_table{
            {"keys", sample.toString()},
            {"loss", sample.loss_},
            {"valid", sample.valid_},
            {"costs", sample.scaled_costs_},
            {"raw_costs", sample.raw_costs_},
            {"enabled", sample.enabled_},
            {"weights", sample.weights_},
            {"use_ols", sample.use_ols_},
            {"similarity", similarity},
            {"zh_stats", zh_stats},
            {"en_stats", en_stats},
        }
    );

    return toml::format(stats);
}

auto Analyzer::analyzeZhKeyCost(Sample& sample) noexcept -> void {
    zh_kc_metric_.analyze(sample, Resources::ZH_KC_DATA);
    sample.raw_costs_[0] = zh_kc_metric_.cost_;
    sample.valid_[0] = zh_kc_metric_.valid_;
}

auto Analyzer::analyzeEnKeyCost(Sample& sample) noexcept -> void {
    en_kc_metric_.analyze(sample, Resources::EN_KC_DATA);
    sample.raw_costs_[1] = en_kc_metric_.cost_;
    sample.valid_[1] = en_kc_metric_.valid_;
}

auto Analyzer::analyzeZhDisCost(Sample& sample) noexcept -> void {
    zh_dc_metric_.analyze(sample, Resources::ZH_DC_DATA);
    sample.raw_costs_[2] = zh_dc_metric_.cost_;
    sample.valid_[2] = zh_dc_metric_.valid_;
}

auto Analyzer::analyzeEnDisCost(Sample& sample) noexcept -> void {
    en_dc_metric_.analyze(sample, Resources::EN_DC_DATA);
    sample.raw_costs_[3] = en_dc_metric_.cost_;
    sample.valid_[3] = en_dc_metric_.valid_;
}

auto Analyzer::analyzeZhSeqCost(Sample& sample) noexcept -> void {
    zh_sc_metric_.analyze(sample, Resources::ZH_SC_DATA);
    sample.raw_costs_[4] = zh_sc_metric_.cost_;
    sample.valid_[4] = zh_sc_metric_.valid_;
}

auto Analyzer::analyzeEnSeqCost(Sample& sample) noexcept -> void {
    en_sc_metric_.analyze(sample, Resources::EN_SC_DATA);
    sample.raw_costs_[5] = en_sc_metric_.cost_;
    sample.valid_[5] = en_sc_metric_.valid_;
}

}
