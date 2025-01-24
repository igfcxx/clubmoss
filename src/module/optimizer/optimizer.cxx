#include "optimizer.hxx"

namespace clubmoss {

auto Optimizer::run() -> void {
    pool_.setSize(Resources::STATUS.at("pool_size").as_integer());
    best_loss_ = std::numeric_limits<fz>::max();
    curr_pool_ = best_pool_ = 0;

    spdlog::info("Optimizing...");

    while (curr_pool_ < MAX_POOLS) {
        const fz curr_loss = pool_.search();
        if (curr_loss < best_loss_) {
            best_pool_ = curr_pool_;
            best_loss_ = curr_loss;
        }
        spdlog::info(
            "[Pool {: >2d}]: current loss = {:8.5f}, "
            "best loss so far is {:8.5f} of Pool {: >2d}",
            curr_pool_, curr_loss, best_loss_, best_pool_
        );
        copyBestSamples();
        stagnation_pools_ = curr_pool_ - best_pool_;
        if (stagnation_pools_ >= max_stagnation_pools_) {
            break;
        }
        ++curr_pool_;
    }

    analyzeSamples();
    saveResults();
}

auto Optimizer::copyBestSamples() -> void {
    for (uz i = 0; i < 30; ++i) {
        const Sample& sample = *pool_.samples_[i].get();
        if (std::ranges::find(best_samples_, sample) == best_samples_.end()) {
            best_samples_.emplace_back(Sample(std::ref(sample)));
        }
    }
}

auto Optimizer::analyzeSamples() -> void {
    for (Sample& sample : best_samples_) {
        analyzer_.analyze(sample);
    }
    pool_.sortSamples();
}

auto Optimizer::saveResults() -> void {
    std::ofstream os;
    for (uz i = 0; i < 5; ++i) {
        const Toml toml = analyzer_.analyze(best_samples_[i]);
        const std::string sub_path = "cache/result/" + std::to_string(i + 1) + ".toml";
        os.open(Utils::absPath(sub_path), std::ios::out);
        os << toml::format(toml);
        os.close();
    }
}

}
