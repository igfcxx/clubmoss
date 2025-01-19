#include "seq_cost_data.hxx"

namespace clubmoss::metric::seq_cost {

Data::Data(const Toml& bigram_data, const Toml& trigram_data) {
    for (const auto& [i, node] : bigram_data.as_table()
         | std::views::take(MAX_RECORDS)
         | std::views::enumerate) {
        validateRecord(node.first, 2, bigram_data, i + 1);
        bigram_records_.emplace_back(node);
    }
    for (const auto& [i, node] : trigram_data.as_table()
         | std::views::take(MAX_RECORDS)
         | std::views::enumerate) {
        validateRecord(node.first, 3, trigram_data, i + 1);
        trigram_records_.emplace_back(node);
    }
    std::ranges::sort(bigram_records_, std::greater<Bigram>());
    std::ranges::sort(trigram_records_, std::greater<Trigram>());
}

auto Data::validateRecord(
    const std::string_view ngram, const uz n,
    const Toml& data, const uz line
) -> void {
    // 字段名 (键值) 的长度应当为 n
    if (ngram.size() != n) {
        throw IllegalData(
            std::format(
                "illegal field\n --> {:s}\n"
                "line {:d}: expect length = {:d}, got {:d}",
                data.location().file_name(), line, n, ngram.size()
            )
        );
    }
    // 字段名的中字符应当合法
    for (const char c : ngram) {
        if (const int cap = std::toupper(c);
            not Utils::isLegalCap(cap)) {
            throw IllegalData(
                std::format(
                    "illegal key code\n --> {:s}\n"
                    "line {:d}: \'{:c}\' is not legal",
                    data.location().file_name(), line, c
                )
            );
        }
    }
    const Toml& value = data.at(ngram.data());
    // [频率]的取值应当在 (1e-5, 0.05) 之间
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
