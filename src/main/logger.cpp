//
// Created by JinHai on 2022/9/30.
//

#include "logger.h"
#include "common/singleton.h"
#include "infinity.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <iostream>
#include <memory>

namespace infinity {

static SharedPtr<spdlog::sinks::stdout_color_sink_mt> stdout_sinker = nullptr;
static SharedPtr<spdlog::sinks::rotating_file_sink_mt> rotating_file_sinker = nullptr;

SharedPtr<spdlog::logger> infinity_logger = nullptr;

void Logger::Initialize(const Config *config_ptr) {
    try {
        if (stdout_sinker == nullptr) {
            stdout_sinker = MakeShared<spdlog::sinks::stdout_color_sink_mt>(); // NOLINT
        }

        SizeT log_max_size = config_ptr->log_max_size();
        SizeT log_file_rotate_count = config_ptr->log_file_rotate_count();

        if (rotating_file_sinker == nullptr) {
            rotating_file_sinker = MakeShared<spdlog::sinks::rotating_file_sink_mt>(*config_ptr->log_file_path(),
                                                                                    log_max_size,
                                                                                    log_file_rotate_count); // NOLINT
        }

        Vector<spdlog::sink_ptr> sinks{stdout_sinker, rotating_file_sinker};

        infinity_logger = MakeShared<spdlog::logger>("infinity", sinks.begin(), sinks.end()); // NOLINT
        spdlog::register_logger(infinity_logger);

        spdlog::set_level(config_ptr->log_level());

    } catch (const std::exception &e) {
        std::cerr << "Initialize logger failed: " << e.what() << std::endl; // NOLINT
        throw e;
    }
}

void Logger::Shutdown() {
    if (stdout_sinker != nullptr && rotating_file_sinker != nullptr) {
        spdlog::shutdown();
        stdout_sinker = nullptr;
        rotating_file_sinker = nullptr;
    }
}

} // namespace infinity