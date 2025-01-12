#ifndef CLUBMOSS_METRIC_HXX
#define CLUBMOSS_METRIC_HXX

#include "../layout/layout.hxx"

namespace clubmoss {

struct MetricConcept {
    virtual ~MetricConcept() = default;
    virtual auto analyze(const Layout&) -> void = 0;
    virtual auto measure(const Layout&) -> fz = 0;
    virtual auto check(const Layout&) -> bool = 0;
};

template <typename T>
struct MetricModel final : MetricConcept {
    explicit MetricModel(T&& t): metric_{std::forward<T>(t)} {}
    auto analyze(const Layout& layout) -> void override { return metric_.analyze(layout); }
    auto measure(const Layout& layout) -> fz override { return metric_.measure(layout); }
    auto check(const Layout& layout) -> bool override { return metric_.check(layout); }

private:
    T metric_;
};

class Metric {
public:
    template <typename T>
    explicit Metric(T&& t): impl_{new MetricModel<T>(std::forward<T>(t))} {}

    auto analyze(const Layout& layout) const -> void { return impl_->analyze(layout); }
    auto measure(const Layout& layout) const -> fz { return impl_->measure(layout); }
    auto check(const Layout& layout) const -> bool { return impl_->check(layout); }

private:
    std::unique_ptr<MetricConcept> impl_;
};

}

#endif // CLUBMOSS_METRIC_HXX
