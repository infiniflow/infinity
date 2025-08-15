module;

#ifdef CI
export module log_helper;

import infinity_core;
#else
export module infinity_core:ut.log_helper;

import :stl;
import :logger;
#endif

export namespace infinity {
struct LogHelper {
    void Reset() {
        LOG_INFO(std::move(oss).str());
        oss.str("");
        oss.clear();
    }
    ~LogHelper() { LOG_INFO(std::move(oss).str()); }
    std::ostringstream oss;
};
}; // namespace infinity
