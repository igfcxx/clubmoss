#ifndef CLUBMOSS_SAMPLE_HXX
#define CLUBMOSS_SAMPLE_HXX

#include "../../layout/layout.hxx"

namespace clubmoss {

class Sample : public Layout {
public:
    explicit Sample(const Layout& layout);
    explicit Sample(Layout&& layout);

    auto update() -> void;

    auto getLoss() const -> fz;
    auto setCost(fz cost, uz index) -> void;
    static auto enabled(uz index) -> bool;

    static auto loadWeights(const Toml& cfg) -> void;
    static auto loadFactors(const Toml& cfg) -> void;

protected:
    fz loss_{std::numeric_limits<fz>::max()};

    static constexpr uz METRICS = MetricId::_size() * Language::_size();

    std::array<fz, METRICS> scaled_costs_{};
    std::array<fz, METRICS> raw_costs_{};

private:
    inline static std::array<fz, METRICS> biases_{};
    inline static std::array<fz, METRICS> ranges_{};

    inline static std::array<bool, METRICS> enabled_{};
    inline static std::array<fz, METRICS> weights_{};
    inline static bool use_ols_ = true;

    static auto fetchWeight(const Toml& node) -> fz;

    static constexpr char WHAT[]{"Illegal weight config: {:s}"};
    using IllegalCfg = IllegalToml<WHAT>;
};

}

#endif //CLUBMOSS_SAMPLE_HXX
