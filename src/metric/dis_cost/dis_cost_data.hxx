#ifndef CLUBMOSS_DIS_COST_DATA_HXX
#define CLUBMOSS_DIS_COST_DATA_HXX

#include "../metric_config.hxx"

namespace clubmoss::metric::dis_cost {

using Node = std::pair<const std::string, const Toml&>;

using Op = struct OrderedPair final {
    Cap src{0};
    Cap dst{0};
    fz f{0.0};

    OrderedPair() = default;

    explicit OrderedPair(const Node& node)
        : f(node.second.as_floating()) {
        src = static_cast<Cap>(std::toupper(node.first[0]));
        dst = static_cast<Cap>(std::toupper(node.first[1]));
    }

    auto operator<=>(const OrderedPair& other) const noexcept -> std::weak_ordering {
        if (this->f > other.f) { return std::weak_ordering::greater; }
        if (this->f < other.f) { return std::weak_ordering::less; }
        return std::weak_ordering::equivalent;
    }
};

class Data final {
public:
    Data() = delete;

    explicit Data(const Toml& data);

    static constexpr uz MAX_RECORDS = 250;

protected:
    std::vector<Op> records_{};

private:
    static auto validateRecord(std::string_view pair, const Toml& data, uz line) -> void;

    static constexpr char WHAT[]{"Illegal pair-frequency data: {:s}"};
    using IllegalData = IllegalToml<WHAT>;

    friend class clubmoss::metric::DisCost;
};

}

#endif // CLUBMOSS_DIS_COST_DATA_HXX
