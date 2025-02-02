#include <omp.h>
#include "o_pool.hxx"

namespace clubmoss::optimizer {

Pool::Pool() {
    samples_.reserve(4800);
    for (uz i = 0; i < size_; ++i) {
        samples_.emplace_back(std::make_unique<Sample>(mgr_.create()));
    }
}

auto Pool::search() noexcept -> fz {
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
        ++curr_epoch_;

        if (curr_epoch_ % 5 == 0) {
            unique();
        }

        updateAndEvaluateSamples();
        sortSamples();
    }

    updateMse();
    spdlog::debug(
        "Epochs: {: >3d} - {: >3d} + {: >3d}, stagnation = {:7.3f}",
        curr_epoch_, best_epoch_, stagnation_epochs_,
        fz(stagnation_epochs_) / fz(curr_epoch_) * 100.0
    );

    return best_loss_;
}

auto Pool::reinitAndEvaluateSamples() noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = 0; i < size_; ++i) {
        mgr_.reinit(*samples_[i]);
        evl_.analyze(*samples_[i]);
    }
}

auto Pool::updateAndEvaluateSamples() noexcept -> void {
    #pragma omp parallel for schedule(guided) shared(samples_) firstprivate(mgr_, evl_) lastprivate(mgr_, evl_) default (none)
    for (uz i = half_; i < size_; ++i) {
        mgr_.mutate(*samples_[i], *samples_[i - half_]);
        evl_.analyze(*samples_[i]);
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

auto Pool::unique() -> void {
    for (uz i = 0; i < half_ - 1; ++i) {
        if (samples_[i].get() == samples_[i + 1].get()) {
            mgr_.reinit(*samples_[i]);
            evl_.analyze(*samples_[i]);
        }
    }
    sortSamples();
}

auto Pool::updateMse() -> void {
    const uz new_value = static_cast<uz>(
        static_cast<fz>(max_stagnation_epochs_) * ALPHA +
        static_cast<fz>(best_epoch_) * (1.0 - ALPHA)
    );
    max_stagnation_epochs_ = std::clamp(new_value, 30uz, 300uz);
}

auto Pool::setSize(const uz size) noexcept -> void {
    assert(size % 2 == 0);
    half_ = size / 2;
    size_ = size;
}

}
