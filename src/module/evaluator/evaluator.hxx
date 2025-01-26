#ifndef CLUBMOSS_EVALUATOR_HXX
#define CLUBMOSS_EVALUATOR_HXX

#include "sample.hxx"
#include "../../common/resources.hxx"
#include "../../metric/key_cost/key_cost.hxx"
#include "../../metric/dis_cost/dis_cost.hxx"
#include "../../metric/seq_cost/seq_cost.hxx"

namespace clubmoss {

class Evaluator {
public:
    Evaluator();

    Evaluator(const Evaluator&);
    Evaluator& operator=(const Evaluator&);

    auto measure(Sample& sample) const noexcept -> void;
    auto analyze(Sample& sample) const noexcept -> void;

    auto evaluate(Sample& sample) const noexcept -> std::string;

    auto measure(Sample& sample, uz task_id) const noexcept -> void;

protected:
    std::vector<Metric> metrics_;

    auto initMetrics() -> void;

private:
    inline static std::array<bool, TASK_COUNT> enabled_{};

    static auto loadEnabledFlags() -> void;

    inline static const bool dummy = (
        loadEnabledFlags(), true
    );

};

}

#endif //CLUBMOSS_EVALUATOR_HXX
