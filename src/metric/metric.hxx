#ifndef CLUBMOSS_METRIC_HXX
#define CLUBMOSS_METRIC_HXX

#include "../layout/layout.hxx"

namespace clubmoss {

struct MetricConcept {
    virtual ~MetricConcept() = default;
    virtual auto measure(const Layout&) -> fz = 0;
    virtual auto analyze(const Layout&) -> std::pair<fz, uz> = 0;
    virtual auto scan(const Layout&, Toml& stats) -> std::pair<fz, uz> = 0;
};

template <typename T>
struct MetricModel final : MetricConcept {
    explicit MetricModel(T&& t): metric_{std::forward<T>(t)} {}
    auto measure(const Layout& layout) -> fz override { return metric_.measure(layout); }
    auto analyze(const Layout& layout) -> std::pair<fz, uz> override { return metric_.analyze(layout); }
    auto scan(const Layout& layout, Toml& stats) -> std::pair<fz, uz> override { return metric_.scan(layout, stats); }

private:
    T metric_;
};

class Metric {
public:
    template <typename T> explicit Metric(T&& t): impl_{new MetricModel<T>(std::forward<T>(t))} {}
    auto measure(const Layout& layout) const -> fz { return impl_->measure(layout); }
    auto analyze(const Layout& layout) const -> std::pair<fz, uz> { return impl_->analyze(layout); }
    auto scan(const Layout& layout, Toml& stats) const -> std::pair<fz, uz> { return impl_->scan(layout, stats); }

private:
    std::unique_ptr<MetricConcept> impl_;
};

namespace metric {
    class KeyCost;
    class DisCost;
    class SeqCost;
}

class Evaluator;

}

#endif //CLUBMOSS_METRIC_HXX
