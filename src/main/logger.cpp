//
// Created by jinhai on 23-10-15.
//

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
    try {
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
        RegisterLogger(infinity_logger);

        SetLogLevel(config_ptr->log_level());

        LOG_TRACE("Logger is initialized.");
    } catch (const StlException &e) {
        Printf("Initialize logger failed: {}", e.what());
        throw e;
    }
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