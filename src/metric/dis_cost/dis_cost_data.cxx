#include "dis_cost_data.hxx"

namespace clubmoss::metric::dis_cost {

Data::Data(const Toml& data) {
    for (const auto& [i, node] : data.as_table() | std::views::enumerate | std::views::take(SIZE)) {
        validateLine(node); records_[i] = Op(node);
    }
}

auto Data::validateLine(const Node& node) -> void {
    const std::string& key = node.first;
    const Toml& value      = node.second;
    // 字段名 (键值) 的长度应当为 2
    if (key.size() != 2) {
        throw IllegalRecord(
            "illegal field",
            value, "name of this field",
            "should be 2"
        );
    }
    // [频率]的取值应当在 (0, 0.10] 之间
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
