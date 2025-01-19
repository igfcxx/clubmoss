#include "key_cost_data.hxx"

namespace clubmoss::metric::key_cost {

Data::Data(const Toml& data) {
    for (const auto& node : data.as_table()) { validateLine(node); }
    for (const auto& [i, node] : data.as_table() | std::views::enumerate) {
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
}

auto Data::validateLine(const std::pair<const std::string, const Toml&>& node) -> void {
    const std::string& key = node.first;
    const Toml& value = node.second;
    // 字段名的长度应当为 1
    if (key.size() != 1) {
        throw IllegalData(
            "illegal field",
            value, "expect length = 1"
        );
    }
    // [键值]应当合法
    if (const int cap = std::toupper(key[0]);
        not Utils::isLegalCap(cap)) {
        throw IllegalData(
            "illegal key code",
            value, "in the name of this field",
            "should be a capital letter or one of "
            "the 4 symbols: ',', '.', ';' and '/'"
        );
    }
    // [频率]的取值应当在 (0, 0.15] 之间
    // 这是一个经验值, 可以根据实际情况调整
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
