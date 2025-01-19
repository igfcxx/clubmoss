#include "dis_cost_data.hxx"

namespace clubmoss::metric::dis_cost {

Data::Data(const Toml& data) {
    for (const auto& [i, node] : data.as_table()
         | std::views::take(MAX_RECORDS)
         | std::views::enumerate) {
        validateRecord(node.first, data, i + 1);
        records_.emplace_back(node);
    }
    std::ranges::sort(records_, std::greater<OrderedPair>());
}

auto Data::validateRecord(const std::string_view pair, const Toml& data, const uz line) -> void {
    // 字段名的长度应当为 2
    if (pair.size() != 2) {
        throw IllegalData(
            std::format(
                "illegal field\n --> {:s}\n"
                "line {:d}: expect length = 2, got {:d}",
                data.location().file_name(), line, pair.size()
            )
        );
    }
    // 字段名的中字符应当合法
    for (const char c : pair) {
        if (const int cap = std::toupper(c);
            not Utils::isLegalCap(cap)
            and cap != ' ') {
            throw IllegalData(
                std::format(
                    "illegal key code\n --> {:s}\n"
                    "line {:d}: \'{:c}\' is not legal",
                    data.location().file_name(), line, c
                )
            );
        }
    }

    const Toml& value = data.at(pair.data());
    // [频率]的取值应当在 (0, 0.05) 之间
    // 这是一个经验值, 可以根据实际情况调整
    const double freq = value.as_floating();
    if (freq < 1e-5) {
        throw IllegalData(
            "illegal frequency",
            value, "should be greater than 1e-5"
        );
    }
    if (freq > 0.05) {
        throw IllegalData(
            "abnormal frequency",
            value, "should be less than 5%"
        );
    }
}

}
