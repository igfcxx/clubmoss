#include "p_pool.hxx"

namespace clubmoss::preprocessor {

auto Pool::search4min(const uz task_id) -> fz {
    best_loss_ = std::numeric_limits<fz>::max();
    curr_epoch_ = best_epoch_ = 0;

    reinitAndEvaluateSamples(task_id);
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
        ++curr_epoch_;
        updateAndEvaluateSamples(task_id);
        sortSamplesAsc();
    }

    updateMse();
    spdlog::debug(
        "Epochs: {: >3d} - {: >3d} + {: >3d}, stagnation = {:7.3f}",
        curr_epoch_, best_epoch_, stagnation_epochs_,
        fz(stagnation_epochs_) / fz(curr_epoch_) * 100.0
    );

    return best_loss_;
}

auto Pool::search4max(const uz task_id) -> fz {
    best_loss_ = std::numeric_limits<fz>::min();
    curr_epoch_ = best_epoch_ = 0;

    reinitAndEvaluateSamples(task_id);
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
        ++curr_epoch_;
        updateAndEvaluateSamples(task_id);
        sortSamplesDesc();
    }

    updateMse();
    spdlog::debug(
        "Epochs: {: >3d} - {: >3d} + {: >3d}, stagnation = {:7.3f}",
        curr_epoch_, best_epoch_, stagnation_epochs_,
        fz(stagnation_epochs_) / fz(curr_epoch_) * 100.0
    );

    return best_loss_;
}

auto Pool::reinitAndEvaluateSamples(const uz task_id) noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_, task_id) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = 0; i < size_; ++i) {
        mgr_.reinit(*samples_[i]);
        evl_.measure(*samples_[i], task_id);
    }
}

auto Pool::updateAndEvaluateSamples(const uz task_id) noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_, task_id) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = half_; i < size_; ++i) {
        mgr_.mutate(*samples_[i], *samples_[i - half_]);
        evl_.measure(*samples_[i], task_id);
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
