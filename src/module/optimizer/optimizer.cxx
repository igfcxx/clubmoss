#include "optimizer.hxx"

namespace clubmoss {

auto Optimizer::search() -> void {
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

    spdlog::info(
        "Optimization complete. Found {:d} candidate solutions.",
        best_samples_.size()
    );
    saveResults();
    saveBaselines();
}

auto Optimizer::copyBestSamples() -> void {
    uz count = 0;
    for (uz i = 0; i < 100; ++i) {
        if (const Sample& sample = *pool_.samples_[i];
            std::ranges::find(best_samples_, sample) == best_samples_.end()) {
            best_samples_.emplace_back(sample);
            if (++count >= 30) { break; }
        }
    }
}

auto Optimizer::saveBaselines() -> void {
    std::ofstream os;
    using namespace std::filesystem;
    const std::string base_path = Utils::absPath("cache/result/baseline");
    if (not exists(base_path)) create_directory(base_path);

    for (const auto& [i, baseline] : layout::baselines::ALL | std::views::enumerate) {
        Sample sample(baseline);
        const std::string stats = pool_.evl_.evaluate(sample);
        const std::string file_name = std::format("{}/baseline_{}.toml", base_path, i + 1);
        os.open(file_name, std::ios::out);
        os << stats;
        os.close();
    }
}

auto Optimizer::saveResults() -> void {
    std::ranges::sort(
        best_samples_, [](const Sample& lhs, const Sample& rhs) {
            return lhs.getLoss() < rhs.getLoss();
        }
    );

    std::ofstream os;
    using namespace std::filesystem;
    const std::string base_path = Utils::absPath("cache/result");
    if (not exists(base_path)) create_directory(base_path);

    uz saved_samples = 0;
    std::vector<std::string> base_keys;

    for (Sample& sample : best_samples_) {
        const std::string layout = sample.toString();
        const std::string base_key = layout.substr(10, 4) + layout.substr(16, 4);
        if (std::ranges::find(base_keys, base_key) != base_keys.end()) {
            continue;
        }

        const std::string stats = pool_.evl_.evaluate(sample);
        const std::string file_name = std::format("{}/result_{}.toml", base_path, ++saved_samples);
        os.open(file_name, std::ios::out);
        os << stats;
        os.close();

        if (saved_samples >= 5) { break; }
    }
    spdlog::info("Saved best {:d} samples...", saved_samples);
}

}
