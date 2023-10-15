//
// Created by jinhai on 23-10-15.
//


module;

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"

#include <memory>

module logger;

//import stl;
//import config;
//import third_party;

namespace infinity {

static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> stdout_sinker = nullptr;
static std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> rotating_file_sinker = nullptr;

std::shared_ptr<spdlog::logger> infinity_logger = nullptr;

void Logger::Initialize(const Config *config_ptr) {
#if 0
    try {
        if (stdout_sinker == nullptr) {
            stdout_sinker = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); // NOLINT
        }

        size_t log_max_size = config_ptr->log_max_size();
        size_t log_file_rotate_count = config_ptr->log_file_rotate_count();

        if (rotating_file_sinker == nullptr) {
            rotating_file_sinker = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(*config_ptr->log_file_path(),
                                                                                    log_max_size,
                                                                                    log_file_rotate_count); // NOLINT
        }

        std::vector<spdlog::sink_ptr> sinks{stdout_sinker, rotating_file_sinker};

        infinity_logger = std::make_shared<spdlog::logger>("infinity", sinks.begin(), sinks.end()); // NOLINT
        spdlog::register_logger(infinity_logger);

        spdlog::set_level(config_ptr->log_level());

    } catch (const std::exception &e) {
//        Printf("Initialize logger failed: {}", e.what());
        throw e;
    }
#endif
}

void Logger::Shutdown() {
    if (stdout_sinker != nullptr && rotating_file_sinker != nullptr) {
        spdlog::shutdown();
        stdout_sinker = nullptr;
        rotating_file_sinker = nullptr;
    }
}

} // namespace infinity