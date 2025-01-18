#ifndef CLUBMOSS_KEY_COST_DATA_HXX
#define CLUBMOSS_KEY_COST_DATA_HXX

#include "../metric.hxx"

namespace clubmoss::metric {
class KeyCost;
}

namespace clubmoss::metric::key_cost {

struct Char final {
    Cap cap{};
    fz freq{};

    Char() = default;

    Char(const Cap c, const fz f) : cap(c), freq(f) {}
};

class Data final {
public:
    Data() = delete;

    explicit Data(const Toml& data);

protected:
    std::array<Cap, KEY_COUNT> caps_{};
    std::array<fz, KEY_COUNT> freq_{};

private:
    static auto validateLine(const std::pair<const std::string, const Toml&>& node) -> void;

    static constexpr char WHAT[]{"Illegal char frequency data: {:s}"};
    using IllegalRecord = IllegalToml<WHAT>;

    friend class clubmoss::metric::KeyCost;
};

}

#endif // CLUBMOSS_KEY_COST_DATA_HXX
