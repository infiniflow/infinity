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

export class Logger {
public:
    static void
    Initialize(Config* config_ptr);

    static void
    Shutdown();
};

export inline bool SHOULD_LOG_TRACE() { return infinity_logger->should_log(spdlog::level::level_enum::trace); }

export inline bool SHOULD_LOG_DEBUG() { return infinity_logger->should_log(spdlog::level::level_enum::debug); }

export inline bool SHOULD_LOG_INFO() { return infinity_logger->should_log(spdlog::level::level_enum::info); }

export inline void
LOG_TRACE(const String& msg) {
    infinity_logger->trace(msg);
}

export inline void
LOG_DEBUG(const String& msg) {
    infinity_logger->debug(msg);
}

export inline void
LOG_INFO(const String& msg) {
    infinity_logger->info(msg);
}

export inline void
LOG_WARN(const String& msg) {
    infinity_logger->warn(msg);
}

export inline void
LOG_ERROR(const String& msg) {
    infinity_logger->error(msg);
}

export inline void
LOG_CRITICAL(const String& msg) {
    infinity_logger->critical(msg);
}

}
