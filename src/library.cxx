#include <omp.h>
#include "library.hxx"

std::shared_ptr<spdlog::logger> logger;

int search(const int threads) {
    try {
        omp_set_num_threads(threads);
        clubmoss::Optimizer o;
        o.search();
    } catch (std::exception& e) {
        spdlog::error("{}", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        spdlog::error("Unknown error occurred.");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int preprocess(const int threads) {
    try {
        omp_set_num_threads(threads);
        clubmoss::Preprocessor p;
        p.run();
    } catch (std::exception& e) {
        spdlog::error("{}", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        spdlog::error("Unknown error occurred.");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void set_log_callback(void (*callback)(const char*)) {
    static auto sink = std::make_shared<clubmoss::LogSink>(
        [callback](const std::string& msg) -> void {
            if (callback) { callback(msg.c_str()); }
        }
    );

    if (not logger) {
        logger = std::make_shared<spdlog::logger>("Logger", sink);
        spdlog::register_logger(logger);
    } else {
        logger->sinks().push_back(sink);
    }
}
