#include "key_cost_data.hxx"

namespace clubmoss::metric::key_cost {

Data::Data(const Toml& data) {
    for (const auto& [i, node] : data.as_table() | std::views::enumerate) {
        validateLine(node.first, data, i + 1);
        caps_[i] = static_cast<Cap>(std::toupper(node.first[0]));
        freq_[i] = static_cast<fz>(node.second.as_floating());
    }
    // 所有按键的频率之和应该为 1.0
    if (const fz sum = Utils::sum(freq_);
        std::abs(sum - 1.0) > 1e-3) {
        throw IllegalData(
            std::format(
                "bad frequency sum\n --> {:s}\n"
                "expect frequency sum = 1.0, got {:.4f}",
                data.location().file_name(), sum
            )
        );
    }
    // 应当包含所有按键, 不重不漏
    auto all_caps = caps_;
    std::ranges::sort(all_caps);
    for (uz i = 0; i < KEY_COUNT - 1; ++i) {
        if (all_caps[i] == all_caps[i + 1]) {
            throw IllegalData(
                std::format(
                    "duplicate character '{:c}'\n --> {:s}\n",
                    all_caps[i], data.location().file_name()
                )
            );
        }
    }
}

auto Data::validateLine(const std::string_view ch, const Toml& data, const uz line) -> void {
    // 字段名的长度应当为 1
    if (ch.size() != 1) {
        throw IllegalData(
            std::format(
                "illegal field {:s}\n --> {:s}\n"
                "line {:d}: expect string length = 2, got {:d}",
                ch, data.location().file_name(), line, ch.size()
            )
        );
    }
    // [键值]应当合法
    if (const int cap = std::toupper(ch[0]);
        not Utils::isLegalCap(cap)) {
        throw IllegalData(
            std::format(
                "illegal key code\n --> {:s}\n"
                "line {:d}: illegal character '{:c}'",
                data.location().file_name(), line, ch[0]
            )
        );
    }
    // [频率]的取值应当在 (0, 0.15] 之间
    // 这是一个经验值, 可以根据实际情况调整
    const Toml& value = data.at(ch.data());
    const double freq = value.as_floating();
    if (freq < 1e-5) {
        throw IllegalData(
            "illegal frequency",
            value, "should be greater than 1e-5"
        );
    }
    if (freq > 0.15) {
        throw IllegalData(
            "abnormal frequency",
            value, "should be less than 15%"
        );
    }
}

}
