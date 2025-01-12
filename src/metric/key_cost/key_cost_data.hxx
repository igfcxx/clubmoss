#ifndef CLUBMOSS_KEY_COST_DATA_HXX
#define CLUBMOSS_KEY_COST_DATA_HXX

#include "../metric.hxx"

namespace clubmoss::metric {
class KeyCost;
}

namespace clubmoss::metric::key_cost {

class Data final {
public:
    Data() = delete;

    explicit Data(const Toml& data);

protected:
    std::array<fz, MAX_KEY_CODE> char_freq_{}; // 按键的频率

private:
    static auto validateLine(const Toml& table, std::string_view key) -> void;

    static constexpr char WHAT[]{"Illegal char frequency data: {:s}"};
    using IllegalRecord = IllegalToml<WHAT>;

    friend class clubmoss::metric::KeyCost;
};

}

#endif // CLUBMOSS_KEY_COST_DATA_HXX
