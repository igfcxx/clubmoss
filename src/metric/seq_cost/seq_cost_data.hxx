#ifndef CLUBMOSS_SEQ_COST_DATA_HXX
#define CLUBMOSS_SEQ_COST_DATA_HXX

#include "../../layout/layout.hxx"

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

    explicit Ngram(const Node& node)
        : f(node.second.as_floating()) {
        for (const auto& [c, cap] : std::views::zip(node.first, caps)) {
            cap = static_cast<Cap>(std::toupper(c));
        }
    }

    auto operator<=>(const Ngram& other) const noexcept -> std::weak_ordering {
        if (this->f > other.f) { return std::weak_ordering::greater; }
        if (this->f < other.f) { return std::weak_ordering::less; }
        return std::weak_ordering::equivalent;
    }
};

using Bigram = Ngram<2>;
using Trigram = Ngram<3>;

class Data final {
public:
    Data() = delete;

    explicit Data(const Toml& data);

    static constexpr uz MAX_RECORDS = 100;

protected:
    std::vector<Ngram<2>> bigram_records_{};
    std::vector<Ngram<3>> trigram_records_{};

private:
    static auto validateRecord(std::string_view ngram, uz n, const Toml& data, uz line) -> void;

    static constexpr char WHAT[]{"Illegal n-gram-frequency data: {:s}"};
    using IllegalData = IllegalToml<WHAT>;

    friend class clubmoss::metric::SeqCost;
};

}

#endif // CLUBMOSS_SEQ_COST_DATA_HXX
