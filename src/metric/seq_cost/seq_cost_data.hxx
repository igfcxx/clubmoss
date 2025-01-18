#ifndef CLUBMOSS_SEQ_COST_DATA_HXX
#define CLUBMOSS_SEQ_COST_DATA_HXX

#include "../metric.hxx"

namespace clubmoss::metric {
class SeqCost;
}

namespace clubmoss::metric::seq_cost {

using Node = std::pair<const std::string, const Toml&>;

template <uz N> requires (N <= 4)
struct Ngram final {
    std::array<Cap, N> caps{0};
    fz f{0.0};

    Ngram() = default;

    explicit Ngram(const Node& node) : f(node.second.as_floating()) {
        for (const auto& [c, cap] : std::views::zip(node.first, caps)) {
            cap = static_cast<Cap>(std::toupper(c));
        }
    }
};

using Bigram = Ngram<2>;
using Trigram = Ngram<3>;

class Data final {
public:
    Data() = delete;

    Data(const Toml& bigram_data, const Toml& trigram_data);

    uz SIZE{100};

protected:
    std::array<Ngram<3>, 100> trigram_records_{};
    std::array<Ngram<2>, 100> bigram_records_{};

private:
    static auto validateRecord(const Node& node, uz n) -> void;

    static constexpr char WHAT[]{"Illegal n-gram frequency data: {:s}"};
    using IllegalRecord = IllegalToml<WHAT>;

    friend class clubmoss::metric::SeqCost;
};

}

#endif // CLUBMOSS_SEQ_COST_DATA_HXX
