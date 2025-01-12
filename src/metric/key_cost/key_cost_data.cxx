#include "key_cost_data.hxx"

namespace clubmoss::metric::key_cost {

Data::Data(const Toml& data) {
    char_freq_.fill(0.0);
    for (const auto& [k, v] : data.as_table()) {
        validateLine(data, k);
        const auto c  = std::toupper(k[0]);
        const auto f  = static_cast<fz>(v.as_floating());
        char_freq_[c] = f;
    }
    // 所有按键的频率之和应该为 1.0
    if (const fz sum = Utils::sum(char_freq_);
        std::abs(sum - 1.0) > 1e-3) {
        throw IllegalRecord(
            std::format(
                "bad frequency sum\n --> {:s}\n"
                "expect frequency sum = 1.0, got {:.4f}",
                data.location().file_name(), sum
            )
        );
    }
}

auto Data::validateLine(const Toml& table, const std::string_view key) -> void {
    const Toml& value = table.at(key.data());
    // 字段名 (键值) 应当为单字符
    if (key.size() != 1) {
        throw IllegalRecord(
            std::format("illegal field {:s}", key),
            value, "name of this field",
            "should be a single character"
        );
    }
    // [键值]的取值应当在 CAP_SET 中
    if (not Utils::isLegalCap(std::toupper(key[0]))) {
        throw IllegalRecord(
            std::format("illegal key code {:s}", key),
            value, "in the name of this field",
            "should be a capital letter or one of "
            "the 4 symbols: ',', '.', ';' and '/'"
        );
    }
    // [频率]的取值应当在 (0, 0.15] 之间
    // 这是一个经验值, 可以根据实际情况调整
    const double freq = value.as_floating();
    if (freq < 1e-5) {
        throw IllegalRecord(
            "illegal frequency",
            value, "should be greater than 1e-5"
        );
    }
    if (freq > 0.15) {
        throw IllegalRecord(
            "abnormal frequency",
            value, "should be less than 15%"
        );
    }
}

}
