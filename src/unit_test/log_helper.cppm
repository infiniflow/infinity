module;

export module infinity_core:ut.log_helper;

import :stl;
import :logger;

namespace infinity {
struct LogHelper {
    void Reset() {
        LOG_INFO(std::move(oss).str());
        oss.str("");
        oss.clear();
    }
    ~LogHelper() { LOG_INFO(std::move(oss).str()); }
    OStringStream oss;
};
};
