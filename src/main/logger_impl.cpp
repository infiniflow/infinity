// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module infinity_core:logger.impl;

import :logger;
import :config;

namespace infinity {

static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> stdout_sinker = nullptr;
static std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> rotating_file_sinker = nullptr;

std::shared_ptr<spdlog::logger> infinity_logger = nullptr;

Status Logger::Initialize(Config *config_ptr) {
    size_t log_file_max_size = config_ptr->LogFileMaxSize();
    size_t log_file_rotate_count = config_ptr->LogFileRotateCount();
    bool log_stdout = config_ptr->LogToStdout();

    if (rotating_file_sinker.get() == nullptr) {
        try {
            rotating_file_sinker = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(config_ptr->LogFilePath(),
                                                                                          log_file_max_size,
                                                                                          log_file_rotate_count); // NOLINT
        } catch (const std::exception &e) {
            std::string error_msg = fmt::format("Error to create log sinker, cause: {}", e.what());
            fmt::print("{}", error_msg);
            return Status::UnexpectedError(error_msg);
        }
    }
    std::vector<spdlog::sink_ptr> sinks{rotating_file_sinker};

    if (log_stdout) {
        if (stdout_sinker.get() == nullptr) {
            stdout_sinker = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); // NOLINT
        }
        sinks.push_back(stdout_sinker);
    }
    infinity_logger = std::make_shared<spdlog::logger>("infinity", sinks.begin(), sinks.end()); // NOLINT
    infinity_logger->set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$] %v");
    infinity_logger->flush_on(spdlog::level::warn);
    spdlog::flush_every(std::chrono::seconds(3));
    spdlog::details::registry::instance().register_logger(infinity_logger);
    SetLogLevel(config_ptr->GetLogLevel());
    LOG_TRACE("Logger is initialized.");
    return Status::OK();
}

void Logger::Initialize(const LoggerConfig &config) {
    bool log_stdout = config.log_to_stdout_;
    if (rotating_file_sinker.get() == nullptr) {
        rotating_file_sinker = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(config.log_file_path_,
                                                                                      config.log_file_max_size_,
                                                                                      config.log_file_rotate_count_); // NOLINT
    }
    if (log_stdout) {
        if (stdout_sinker.get() == nullptr) {
            stdout_sinker = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); // NOLINT
        }
        std::vector<spdlog::sink_ptr> sinks{stdout_sinker, rotating_file_sinker};

        infinity_logger = std::make_shared<spdlog::logger>("infinity", sinks.begin(), sinks.end()); // NOLINT
        infinity_logger->set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$] %v");
        spdlog::details::registry::instance().register_logger(infinity_logger);
    } else {
        std::vector<spdlog::sink_ptr> sinks{rotating_file_sinker};
        infinity_logger = std::make_shared<spdlog::logger>("infinity", sinks.begin(), sinks.end()); // NOLINT
        infinity_logger->set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$] %v");
        spdlog::details::registry::instance().register_logger(infinity_logger);
    }
    SetLogLevel(config.log_level_);
}

void Logger::Flush() {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->flush();
    }
}

void Logger::Shutdown() {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->flush();
    }
    if (stdout_sinker.get() != nullptr or rotating_file_sinker.get() != nullptr) {
        spdlog::shutdown();
        stdout_sinker = nullptr;
        rotating_file_sinker = nullptr;
        infinity_logger = nullptr;
    }
}

} // namespace infinity
