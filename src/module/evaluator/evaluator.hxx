#ifndef CLUBMOSS_EVALUATOR_HXX
#define CLUBMOSS_EVALUATOR_HXX

#include"sample.hxx"
#include "../../metric/key_cost/key_cost.hxx"
#include "../../metric/dis_cost/dis_cost.hxx"
#include "../../metric/seq_cost/seq_cost.hxx"

namespace clubmoss {

class Evaluator {
public:
    auto evaluate(Sample& sample) -> void;

private:
    inline static auto parse = [](const std::string_view sub_path) -> Toml {
        return toml::parse<toml::ordered_type_config>(Utils::absPath(sub_path));
    };

    // @formatter:off //
    inline static const Toml ZH_CHAR_FREQ  = parse("cache/freq/chinese/char.toml");
    inline static const Toml EN_CHAR_FREQ  = parse("cache/freq/english/char.toml");
    inline static const Toml ZH_PAIR_FREQ  = parse("cache/freq/chinese/pair.toml");
    inline static const Toml EN_PAIR_FREQ  = parse("cache/freq/english/pair.toml");
    inline static const Toml ZH_2GRAM_FREQ = parse("cache/freq/chinese/2-gram.toml");
    inline static const Toml EN_2GRAM_FREQ = parse("cache/freq/english/2-gram.toml");
    inline static const Toml ZH_3GRAM_FREQ = parse("cache/freq/chinese/3-gram.toml");
    inline static const Toml EN_3GRAM_FREQ = parse("cache/freq/english/3-gram.toml");
    inline static const Toml METRIC_CONFIG = parse("conf/metric.toml");
    inline static const Toml WEIGHT_CONFIG = parse("conf/weight.toml");
    inline static const Toml STATUS = parse("cache/status.toml");
    // @formatter:on //

    inline static const metric::key_cost::Data ZH_KC_DATA{ZH_CHAR_FREQ};
    inline static const metric::key_cost::Data EN_KC_DATA{EN_CHAR_FREQ};
    inline static const metric::dis_cost::Data ZH_DC_DATA{ZH_PAIR_FREQ};
    inline static const metric::dis_cost::Data EN_DC_DATA{EN_PAIR_FREQ};
    inline static const metric::seq_cost::Data ZH_SC_DATA{ZH_2GRAM_FREQ, ZH_3GRAM_FREQ};
    inline static const metric::seq_cost::Data EN_SC_DATA{EN_2GRAM_FREQ, EN_3GRAM_FREQ};

    static inline const bool dummy = (
        metric::KeyCost::loadCfg(METRIC_CONFIG.at("key_cost")),
        metric::DisCost::loadCfg(METRIC_CONFIG.at("dis_cost")),
        metric::SeqCost::loadCfg(METRIC_CONFIG.at("seq_cost")),
        Sample::loadWeights(WEIGHT_CONFIG),
        Sample::loadFactors(STATUS),
        true
    );

protected:
    metric::KeyCost zh_kc_metric_ = metric::KeyCost(ZH_KC_DATA);
    metric::KeyCost en_kc_metric_ = metric::KeyCost(EN_KC_DATA);
    metric::DisCost zh_dc_metric_ = metric::DisCost(ZH_DC_DATA);
    metric::DisCost en_dc_metric_ = metric::DisCost(EN_DC_DATA);
    metric::SeqCost zh_sc_metric_ = metric::SeqCost(ZH_SC_DATA);
    metric::SeqCost en_sc_metric_ = metric::SeqCost(EN_SC_DATA);
};

}

#endif //CLUBMOSS_EVALUATOR_HXX
