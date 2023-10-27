//
// Created by jinhai on 23-10-15.
//
module;

import stl;
import third_party;

export module logger;

//import config;
//import third_party;
//import stl;

namespace infinity {

export extern SharedPtr<spd_logger> infinity_logger;

class Config;

export class Logger {
public:
    static void
    Initialize(const Config* config_ptr);

    static void
    Shutdown();
};

export inline void
LOG_TRACE(const String& msg) {
    infinity_logger->trace(msg);
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
