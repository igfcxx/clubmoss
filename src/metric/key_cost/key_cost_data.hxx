#ifndef CLUBMOSS_KEY_COST_DATA_HXX
#define CLUBMOSS_KEY_COST_DATA_HXX

#include "../../layout/layout.hxx"

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
    static auto validateLine(std::string_view ch, const Toml& data, uz line) -> void;

    static constexpr char WHAT[]{"Illegal char-frequency data: {:s}"};
    using IllegalData = IllegalToml<WHAT>;

    friend class clubmoss::metric::KeyCost;
};

}

#endif // CLUBMOSS_KEY_COST_DATA_HXX
