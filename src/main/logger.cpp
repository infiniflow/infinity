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

module logger;

import stl;
import config;
import third_party;

namespace infinity {

static SharedPtr<spd_stdout_color_sink> stdout_sinker = nullptr;
static SharedPtr<spd_rotating_file_sink> rotating_file_sinker = nullptr;

SharedPtr<spd_logger> infinity_logger = nullptr;

void Logger::Initialize(const Config *config_ptr) {
    if (stdout_sinker.get() == nullptr) {
        stdout_sinker = MakeShared<spd_stdout_color_sink>(); // NOLINT
    }

    SizeT log_max_size = config_ptr->log_max_size();
    SizeT log_file_rotate_count = config_ptr->log_file_rotate_count();

    if (rotating_file_sinker.get() == nullptr) {
        rotating_file_sinker = MakeShared<spd_rotating_file_sink>(*config_ptr->log_file_path(), log_max_size,
                                                                  log_file_rotate_count); // NOLINT
    }

    Vector<spd_sink_ptr> sinks{stdout_sinker, rotating_file_sinker};

    infinity_logger = MakeShared<spd_logger>("infinity", sinks.begin(), sinks.end()); // NOLINT
    infinity_logger->set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$] %v");
    RegisterLogger(infinity_logger);

    SetLogLevel(config_ptr->log_level());

    LOG_TRACE("Logger is initialized.");
}

void Logger::Shutdown() {
    if (stdout_sinker.get() != nullptr && rotating_file_sinker.get() != nullptr) {
        ShutdownLogger();
        stdout_sinker = nullptr;
        rotating_file_sinker = nullptr;
        infinity_logger = nullptr;
    }
}

} // namespace infinity