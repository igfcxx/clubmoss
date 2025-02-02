#ifndef CLUBMOSS_RESOURCES_HXX
#define CLUBMOSS_RESOURCES_HXX

#include "../layout/layout_manager.hxx"
#include "../metric/key_cost/key_cost.hxx"
#include "../metric/dis_cost/dis_cost.hxx"
#include "../metric/seq_cost/seq_cost.hxx"
#include "../module/evaluator/sample.hxx"

namespace clubmoss {

class Resources {
    inline static auto parse = [](const std::string_view sub_path) -> Toml {
        return toml::parse<toml::ordered_type_config>(Utils::absPath(sub_path));
    };

    // @formatter:off //
    inline static const Toml LAYOUT_CONFIG = parse("conf/layout.toml");
    inline static const Toml METRIC_CONFIG = parse("conf/metric.toml");
    inline static const Toml SCORE_CONFIG  = parse("conf/score.toml");

    inline static const Toml ZH_CHAR_FREQ = parse("data/chinese/char.toml");
    inline static const Toml EN_CHAR_FREQ = parse("data/english/char.toml");
    inline static const Toml ZH_PAIR_FREQ = parse("data/chinese/pair.toml");
    inline static const Toml EN_PAIR_FREQ = parse("data/english/pair.toml");
    inline static const Toml ZH_SEQ_FREQ  = parse("data/chinese/seq.toml");
    inline static const Toml EN_SEQ_FREQ  = parse("data/english/seq.toml");
    // @formatter:on //

public:
    inline static const Toml STATUS = parse("cache/status.toml");

    inline static std::array<metric::key_cost::Data, Language::_size()> KC_DATA{
        metric::key_cost::Data(ZH_CHAR_FREQ), metric::key_cost::Data(EN_CHAR_FREQ)
    };
    inline static std::array<metric::dis_cost::Data, Language::_size()> DC_DATA{
        metric::dis_cost::Data(ZH_PAIR_FREQ), metric::dis_cost::Data(EN_PAIR_FREQ)
    };
    inline static std::array<metric::seq_cost::Data, Language::_size()> SC_DATA{
        metric::seq_cost::Data(ZH_SEQ_FREQ), metric::seq_cost::Data(EN_SEQ_FREQ)
    };

private:
    inline static const bool dummy = (
        layout::Manager::loadCfg(LAYOUT_CONFIG),
        metric::Config::loadCfg(METRIC_CONFIG, SCORE_CONFIG),
        Sample::loadCfg(SCORE_CONFIG, STATUS),
        true
    );
};

}

#endif //CLUBMOSS_RESOURCES_HXX
