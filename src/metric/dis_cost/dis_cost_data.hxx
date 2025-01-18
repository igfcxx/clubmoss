#ifndef CLUBMOSS_DIS_COST_DATA_HXX
#define CLUBMOSS_DIS_COST_DATA_HXX

#include "../metric.hxx"

namespace clubmoss::metric {
class DisCost;
}

namespace clubmoss::metric::dis_cost {

using Node = std::pair<const std::string, const Toml&>;

using Op = struct OrderedPair final {
    Cap src{0};
    Cap dst{0};
    fz f{0.0};

    OrderedPair() = default;

    explicit OrderedPair(const Node& node): f(node.second.as_floating()) {
        src = static_cast<Cap>(std::toupper(node.first[0]));
        dst = static_cast<Cap>(std::toupper(node.first[1]));
    }
};

class Data final {
public:
    Data() = delete;

    explicit Data(const Toml& data);

    uz SIZE{200};

protected:
    std::array<Op, 200> records_{};

private:
    static auto validateLine(const Node& node) -> void;

    static constexpr char WHAT[]{"Illegal pair frequency data: {:s}"};
    using IllegalRecord = IllegalToml<WHAT>;

    friend class clubmoss::metric::DisCost;
};

}

#endif // CLUBMOSS_DIS_COST_DATA_HXX
