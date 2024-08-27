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
module;

export module logger;

import stl;
import third_party;

namespace infinity {

export extern SharedPtr<spdlog::logger> infinity_logger;

class Config;

export struct LoggerConfig {
    bool log_to_stdout_ = true;
    String log_file_path_ = "tmp.log";
    SizeT log_file_max_size_ = 1024 * 1024 * 10;
    SizeT log_file_rotate_count_ = 5;
    LogLevel log_level_ = LogLevel::kInfo;
};

export class Logger {
public:
    static void
    Initialize(Config* config_ptr);

    static void Initialize(const LoggerConfig &config);

    static void
    Shutdown();
};

inline bool IS_LOGGER_INITIALIZED() { return infinity_logger.get() != nullptr; }

export inline bool SHOULD_LOG_TRACE() { return IS_LOGGER_INITIALIZED() && infinity_logger->should_log(spdlog::level::level_enum::trace); }

export inline bool SHOULD_LOG_DEBUG() { return IS_LOGGER_INITIALIZED() && infinity_logger->should_log(spdlog::level::level_enum::debug); }

export inline bool SHOULD_LOG_INFO() { return IS_LOGGER_INITIALIZED() && infinity_logger->should_log(spdlog::level::level_enum::info); }

export inline void
LOG_TRACE(const String& msg) {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->trace(msg);
    } else {
        fmt::print("[trace] {}\n", msg);
    }
}

export inline void
LOG_DEBUG(const String& msg) {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->debug(msg);
    } else {
        fmt::print("[debug] {}\n", msg);
    }
}

export inline void
LOG_INFO(const String& msg) {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->info(msg);
    } else {
        fmt::print("[info] {}\n", msg);
    }
}

export inline void
LOG_WARN(const String& msg) {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->warn(msg);
    } else {
        fmt::print("[warn] {}\n", msg);
    }
}

export inline void
LOG_ERROR(const String& msg) {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->error(msg);
    } else {
        fmt::print("[error] {}\n", msg);
    }
}

export inline void
LOG_CRITICAL(const String& msg) {
    if (IS_LOGGER_INITIALIZED()) {
        infinity_logger->critical(msg);
    } else {
        fmt::print("[critical] {}\n", msg);
    }
}

}
