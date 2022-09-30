//
// Created by JinHai on 2022/9/30.
//

#include "logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "common/singleton.h"
#include "infinity.h"

#include <memory>
#include <iostream>


namespace infinity {



//#include "spdlog/fmt/ostr.h"
//#include "spdlog/sinks/basic_file_sink.h"
//#include "spdlog/sinks/stdout_sinks.h"
//#include "spdlog/spdlog.h"


static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> stdout_sinker = nullptr;
static std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> rotating_file_sinker = nullptr;

std::shared_ptr<spdlog::logger> infinity_logger = nullptr;


void
Logger::Initialize() {
    try {
        if (stdout_sinker == nullptr) {
            stdout_sinker = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();  // NOLINT
        }

        const std::string& log_path = Infinity::instance().config()->option_.log_file;
        std::size_t log_max_size = Infinity::instance().config()->option_.log_max_size;
        std::size_t log_file_rotate_count = Infinity::instance().config()->option_.log_file_rotate_count;
        const std::string& log_level = Infinity::instance().config()->option_.log_level;

        if (rotating_file_sinker == nullptr) {
            rotating_file_sinker = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_path, log_max_size, log_file_rotate_count);  // NOLINT
        }

        std::vector<spdlog::sink_ptr> sinks {stdout_sinker, rotating_file_sinker};

        infinity_logger = std::make_shared<spdlog::logger>("infinity", sinks.begin(), sinks.end());  // NOLINT
        spdlog::register_logger(infinity_logger);

        if(log_level == "trace") {
            spdlog::set_level(spdlog::level::trace);
        } else {
            spdlog::set_level(spdlog::level::info);
        }

    } catch (const std::exception &e) {
        std::cerr << "Initialize logger failed: " << e.what() << std::endl;  // NOLINT
        throw e;
    }
}

void
Logger::Shutdown() {
    if (stdout_sinker != nullptr && rotating_file_sinker != nullptr) {
        spdlog::shutdown();
        stdout_sinker = nullptr;
        rotating_file_sinker = nullptr;
    }
}

}