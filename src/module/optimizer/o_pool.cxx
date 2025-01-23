#include "o_pool.hxx"
#include <omp.h>

namespace clubmoss::optimizer {

Pool::Pool() {
    samples_.reserve(9600);
    for (uz i = 0; i < size_; ++i) {
        samples_.emplace_back(std::make_unique<Sample>(mgr_.create()));
    }
}

auto Pool::search() noexcept -> void {
    best_loss_ = std::numeric_limits<fz>::max();
    curr_epoch_ = best_epoch_ = 0;

    reinitAndEvaluateSamples();
    sortSamples();

    while (curr_epoch_ < MAX_EPOCHS) {
        if (const fz loss = samples_.front()->getLoss(); loss < best_loss_) {
            best_epoch_ = curr_epoch_;
            best_loss_ = loss;
        }
        stagnation_epochs_ = curr_epoch_ - best_epoch_;
        if (stagnation_epochs_ >= max_stagnation_epochs_) {
            break;
        }
        updateAndEvaluateSamples();
        sortSamples();
        ++curr_epoch_;
    }
}

auto Pool::reinitAndEvaluateSamples() noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = 0; i < size_; ++i) {
        mgr_.reinit(*samples_[i]);
        evl_.evaluate(*samples_[i]);
    }
}

auto Pool::updateAndEvaluateSamples() noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = half_; i < size_; ++i) {
        mgr_.mutate(*samples_[i], *samples_[i - half_]);
        evl_.evaluate(*samples_[i]);
    }
}

auto Pool::sortSamples() -> void {
    std::sort(
        &samples_.front(), &samples_[size_ - 1],
        [](const std::unique_ptr<Sample>& lhs, const std::unique_ptr<Sample>& rhs) {
            return lhs->getLoss() < rhs->getLoss();
        }
    );
}

auto Pool::setSize(const uz size) noexcept -> void {
    assert(size % 2 == 0);
    half_ = size / 2;
    size_ = size;
}

}
