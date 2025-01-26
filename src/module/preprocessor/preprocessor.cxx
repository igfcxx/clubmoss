#include "preprocessor.hxx"

namespace clubmoss {

auto Preprocessor::run() -> void {
    searchExtremes();
    estimateSize();
    saveStatus();
}

auto Preprocessor::searchExtremes() -> void {
    for (const MetricId metric : MetricId::_values()) {
        for (const Language language : Language::_values()) {
            minimizeCosts(metric, language);
            maximizeCosts(metric, language);
        }
    }
    for (const MetricId metric : MetricId::_values()) {
        for (const Language language : Language::_values()) {
            const uz task_id = Utils::taskIdOf(metric, language);
            spdlog::info(
                "{} - {}: [{:8.5f}, {:8.5f}]",
                metric._to_string(), language._to_string(), min_costs_[task_id], max_costs_[task_id]
            );
        }
    }
}

auto Preprocessor::minimizeCosts(const MetricId metric, const Language language) -> void {
    const uz task_id = Utils::taskIdOf(metric, language);
    min_costs_[task_id] = std::numeric_limits<fz>::max();
    pool_.max_stagnation_epochs_ = 250;
    curr_pool_ = best_pool_ = 0;
    candidates_.clear();

    spdlog::info(
        "Searching for minimum cost for {} metric in {} statistics...",
        metric._to_string(), language._to_string()
    );

    while (curr_pool_ < MAX_POOLS) {
        const fz min_cost = pool_.search4min(task_id);
        if (min_cost < min_costs_[task_id]) {
            min_costs_[task_id] = min_cost;
            candidates_.insert(min_cost);
            best_pool_ = curr_pool_;
        }
        spdlog::info(
            "[Pool {: >2d}]: found {:8.5f}, current best is {:8.5f} in Pool {: >2d}",
            curr_pool_, min_cost, min_costs_[task_id], best_pool_
        );
        if (curr_pool_ % 5 == 0) {
            max_stagnation_pools_ = std::clamp(
                candidates_.size() * 5uz, 5uz, 15uz
            );
        }
        stagnation_pools_ = curr_pool_ - best_pool_;
        if (stagnation_pools_ >= max_stagnation_pools_) {
            break;
        }
        ++curr_pool_;
    }
}

auto Preprocessor::maximizeCosts(const MetricId metric, const Language language) -> void {
    const uz task_id = Utils::taskIdOf(metric, language);
    max_costs_[task_id] = std::numeric_limits<fz>::min();
    pool_.max_stagnation_epochs_ = 250;
    curr_pool_ = best_pool_ = 0;
    candidates_.clear();

    spdlog::info(
        "Searching for maximum cost for {} metric in {} statistics...",
        metric._to_string(), language._to_string()
    );

    while (curr_pool_ < MAX_POOLS) {
        const fz max_cost = pool_.search4max(task_id);
        if (max_cost > max_costs_[task_id]) {
            max_costs_[task_id] = max_cost;
            candidates_.insert(max_cost);
            best_pool_ = curr_pool_;
        }
        spdlog::info(
            "[Pool {: >2d}]: found {:8.5f}, current best is {:8.5f} in Pool {: >2d}",
            curr_pool_, max_cost, max_costs_[task_id], best_pool_
        );
        if (curr_pool_ % 5 == 0) {
            max_stagnation_pools_ = std::clamp(
                candidates_.size() * 5uz, 5uz, 15uz
            );
        }
        stagnation_pools_ = curr_pool_ - best_pool_;
        if (stagnation_pools_ >= max_stagnation_pools_) {
            break;
        }
        ++curr_pool_;
    }
}

auto Preprocessor::estimateSize() -> void {
    static constexpr std::array<uz, 4> OPTIONAL_SIZES{
        2400, 1200, 600, 300
    };

    best_size_ = 4800;
    const fz best_loss = tryPoolSize(4800);
    for (const uz size : OPTIONAL_SIZES) {
        const fz loss = tryPoolSize(size);
        if (loss > best_loss) return;
        best_size_ = size;
    }

}

auto Preprocessor::tryPoolSize(const uz pool_size) -> fz {
    best_loss_ = std::numeric_limits<fz>::max();
    curr_pool_ = best_pool_ = 0;
    max_stagnation_pools_ = 20;
    pool_.setSize(pool_size);

    spdlog::info("Testing pool of {} samples...", pool_size);

    while (curr_pool_ < MAX_POOLS) {
        const fz curr_loss = pool_.search();
        if (curr_loss < best_loss_) {
            best_pool_ = curr_pool_;
            best_loss_ = curr_loss;
        }
        spdlog::info(
            "[Pool {: >2d}]: found {:8.5f}, current best is {:8.5f} in Pool {: >2d}",
            curr_pool_, curr_loss, best_loss_, best_pool_
        );
        stagnation_pools_ = curr_pool_ - best_pool_;
        if (stagnation_pools_ >= max_stagnation_pools_) {
            break;
        }
        ++curr_pool_;
    }

    return best_loss_;
}

auto Preprocessor::saveStatus() -> void {
    std::array<fz, TASK_COUNT> biases{};
    std::array<fz, TASK_COUNT> ranges{};
    for (uz task = 0; task < TASK_COUNT; ++task) {
        ranges[task] = max_costs_[task] - min_costs_[task];
        biases[task] = min_costs_[task];
    }
    const Toml toml(
        toml::ordered_table{
            {"pool_size", best_size_},
            {"biases", biases},
            {"ranges", ranges},
        }
    );
    std::ofstream os;
    os.open(Utils::absPath("cache/status.toml"), std::ios::out);
    os << toml::format(toml);
    os.close();
}

}
