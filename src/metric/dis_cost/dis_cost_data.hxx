#ifndef CLUBMOSS_DIS_COST_DATA_HXX
#define CLUBMOSS_DIS_COST_DATA_HXX

#include "../metric.hxx"

namespace clubmoss::metric {
class DisCost;
}

namespace clubmoss::metric::dis_cost {

using Op = struct OrderedPair final {
    Cap src{0};
    Cap dst{0};
    fz f{};

    OrderedPair() = default;

    OrderedPair(const std::string_view s, const fz freq)
        : f(freq) {
        src = static_cast<Cap>(std::toupper(s[0]));
        dst = static_cast<Cap>(std::toupper(s[1]));
    }
};

class Data final {
public:
    Data() = delete;

    explicit Data(const Toml& data);

    uz SIZE{200};

protected:
    std::array<Op, 200> records_{};
    std::array<std::array<Cap, 2>, 200> c1_;
    std::array<std::array<Cap, 200>, 2> c2_;
    std::array<fz, 200> freq_{};

private:
    static auto validateLine(const std::pair<const std::string, const Toml&>& node) -> void;

    static constexpr char WHAT[]{"Illegal pair frequency data: {:s}"};
    using IllegalRecord = IllegalToml<WHAT>;

    friend class clubmoss::metric::DisCost;
};

}

#endif // CLUBMOSS_DIS_COST_DATA_HXX
