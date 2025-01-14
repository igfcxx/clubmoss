#include "dis_cost_data.hxx"
#include <iostream>

namespace clubmoss::metric::dis_cost {

Data::Data(const Toml& data) {
    for (const auto& [i, node] : data.as_table() | std::views::enumerate | std::views::take(SIZE)) {
        validateLine(node);
        const std::string key = node.first;

        const auto c1 = std::toupper(key[0]);
        const auto c2 = std::toupper(key[1]);
        const auto f  = static_cast<fz>(node.second.as_floating());
        records_[i]   = Op(node.first, f);
        freq_[i]      = f;
        c1_[i][0]    = c1;
        c1_[i][1]    = c2;
        c2_[0][i]    = c1;
        c2_[1][i]    = c2;
    }
}

auto Data::validateLine(const std::pair<const std::string, const Toml&>& node) -> void {
    const std::string& key = node.first;
    const Toml& value      = node.second;
    // 字段名 (键值) 应当为单字符
    if (key.size() != 2) {
        throw IllegalRecord(
            "illegal field",
            value, "name of this field",
            "should be 2"
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
    if (freq > 0.10) {
        throw IllegalRecord(
            "abnormal frequency",
            value, "should be less than 10%"
        );
    }
}

}
