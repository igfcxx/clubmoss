#ifndef CLUBMOSS_RESOURCES_HXX
#define CLUBMOSS_RESOURCES_HXX

#include "../layout/layout_manager.hxx"
#include "../metric/key_cost/key_cost.hxx"
#include "../metric/dis_cost/dis_cost.hxx"
#include "../metric/seq_cost/seq_cost.hxx"
#include "evaluator/sample.hxx"

namespace clubmoss {

class Resources {
    inline static auto parse = [](const std::string_view sub_path) -> Toml {
        return toml::parse<toml::ordered_type_config>(Utils::absPath(sub_path));
    };

    // @formatter:off //
    inline static const Toml LAYOUT_CONFIG = parse("conf/layout.toml");
    inline static const Toml METRIC_CONFIG = parse("conf/metric.toml");
    inline static const Toml WEIGHT_CONFIG = parse("conf/weight.toml");

    inline static const Toml ZH_CHAR_FREQ = parse("cache/freq/chinese/char.toml");
    inline static const Toml EN_CHAR_FREQ = parse("cache/freq/english/char.toml");
    inline static const Toml ZH_PAIR_FREQ = parse("cache/freq/chinese/pair.toml");
    inline static const Toml EN_PAIR_FREQ = parse("cache/freq/english/pair.toml");
    inline static const Toml ZH_SEQ_FREQ  = parse("cache/freq/chinese/seq.toml");
    inline static const Toml EN_SEQ_FREQ  = parse("cache/freq/english/seq.toml");
    // @formatter:on //

public:
    inline static const Toml STATUS = parse("cache/status.toml");

    inline static const metric::key_cost::Data ZH_KC_DATA{ZH_CHAR_FREQ};
    inline static const metric::key_cost::Data EN_KC_DATA{EN_CHAR_FREQ};
    inline static const metric::dis_cost::Data ZH_DC_DATA{ZH_PAIR_FREQ};
    inline static const metric::dis_cost::Data EN_DC_DATA{EN_PAIR_FREQ};
    inline static const metric::seq_cost::Data ZH_SC_DATA{ZH_SEQ_FREQ};
    inline static const metric::seq_cost::Data EN_SC_DATA{EN_SEQ_FREQ};

    inline static const bool dummy = (
        layout::Manager::loadCfg(LAYOUT_CONFIG),
        metric::KeyCost::loadCfg(METRIC_CONFIG.at("key_cost")),
        metric::DisCost::loadCfg(METRIC_CONFIG.at("dis_cost")),
        metric::SeqCost::loadCfg(METRIC_CONFIG.at("seq_cost")),
        Sample::loadWeights(WEIGHT_CONFIG),
        Sample::loadFactors(STATUS),
        true
    );
};

}

#endif //CLUBMOSS_RESOURCES_HXX
