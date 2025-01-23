#include "p_pool.hxx"

namespace clubmoss::preprocessor {

auto Pool::search4max(const uz metric_idx) -> fz {
    best_loss_ = std::numeric_limits<fz>::min();
    curr_epoch_ = best_epoch_ = 0;

    reinitAndEvaluateSamples(metric_idx);
    sortSamplesDesc();

    while (curr_epoch_ < MAX_EPOCHS) {
        if (const fz loss = samples_.front()->getLoss(); loss > best_loss_) {
            best_epoch_ = curr_epoch_;
            best_loss_ = loss;
        }
        stagnation_epochs_ = curr_epoch_ - best_epoch_;
        if (stagnation_epochs_ >= max_stagnation_epochs_) {
            break;
        }
        updateAndEvaluateSamples(metric_idx);
        sortSamplesDesc();
        ++curr_epoch_;
    }
    return best_loss_;
}

auto Pool::search4min(const uz metric_idx) -> fz {
    best_loss_ = std::numeric_limits<fz>::max();
    curr_epoch_ = best_epoch_ = 0;

    reinitAndEvaluateSamples(metric_idx);
    sortSamplesAsc();

    while (curr_epoch_ < MAX_EPOCHS) {
        if (const fz loss = samples_.front()->getLoss(); loss < best_loss_) {
            best_epoch_ = curr_epoch_;
            best_loss_ = loss;
        }
        stagnation_epochs_ = curr_epoch_ - best_epoch_;
        if (stagnation_epochs_ >= max_stagnation_epochs_) {
            break;
        }
        updateAndEvaluateSamples(metric_idx);
        sortSamplesAsc();
        ++curr_epoch_;
    }
    return best_loss_;
}

auto Pool::reinitAndEvaluateSamples(const uz metric) noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_, metric) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = 0; i < size_; ++i) {
        mgr_.reinit(*samples_[i]);
        evl_.calcCost(*samples_[i], metric);
    }
}

auto Pool::updateAndEvaluateSamples(const uz metric) noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_, metric) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = half_; i < size_; ++i) {
        mgr_.mutate(*samples_[i], *samples_[i - half_]);
        evl_.calcCost(*samples_[i], metric);
    }
}

auto Pool::sortSamplesDesc() -> void {
    std::sort(
        &samples_.front(), &samples_[size_ - 1],
        [](const std::unique_ptr<Sample>& lhs, const std::unique_ptr<Sample>& rhs) {
            return lhs->getLoss() > rhs->getLoss();
        }
    );
}

auto Pool::sortSamplesAsc() -> void {
    std::sort(
        &samples_.front(), &samples_[size_ - 1],
        [](const std::unique_ptr<Sample>& lhs, const std::unique_ptr<Sample>& rhs) {
            return lhs->getLoss() < rhs->getLoss();
        }
    );
}

}
