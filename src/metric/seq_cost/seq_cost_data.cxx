#include "seq_cost_data.hxx"

namespace clubmoss::metric::seq_cost {

Data::Data(const Toml& bigram_data, const Toml& trigram_data) {
    for (const auto& [i, node] : trigram_data.as_table() | std::views::enumerate | std::views::take(SIZE)) {
        validateRecord(node, 3); trigram_records_[i] = Trigram(node);
    }
    for (const auto& [i, node] : bigram_data.as_table() | std::views::enumerate | std::views::take(SIZE)) {
        validateRecord(node, 2); bigram_records_[i] = Bigram(node);
    }
}

auto Data::validateRecord(const Node& node, const uz n) -> void {
    const std::string& key = node.first;
    const Toml& value      = node.second;
    // 字段名 (键值) 的长度应当为 2
    if (key.size() != n) {
        throw IllegalRecord(
            "illegal field",
            value, "name of this field",
            std::format("should be {:d}", n)
        );
    }
    // [频率]的取值应当在 (0, 0.05] 之间
    // 这是一个经验值, 可以根据实际情况调整
    const double freq = value.as_floating();
    if (freq < 1e-5) {
        throw IllegalRecord(
            "illegal frequency",
            value, "should be greater than 1e-5"
        );
    }
    if (freq > 0.05) {
        throw IllegalRecord(
            "abnormal frequency",
            value, "should be less than 5%"
        );
    }
}

}
