#ifndef CLUBMOSS_SEQ_COST_DATA_HXX
#define CLUBMOSS_SEQ_COST_DATA_HXX

#include "../metric_config.hxx"

namespace clubmoss::metric::seq_cost {

class Data final {
public:
    Data() = delete;

    explicit Data(const Toml& data);

    static constexpr uz MAX_RECORDS = 100;

protected:
    std::vector<Ngram<2>> bigram_records_{};
    std::vector<Ngram<3>> trigram_records_{};

private:
    static auto validateRecord(std::string_view ngram, uz n, const Toml& data, uz line) -> void;

    static constexpr char WHAT[]{"Illegal n-gram-frequency data: {:s}"};
    using IllegalData = IllegalToml<WHAT>;

    friend class clubmoss::metric::SeqCost;
};

}

#endif // CLUBMOSS_SEQ_COST_DATA_HXX
