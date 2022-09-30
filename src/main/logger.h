//
// Created by JinHai on 2022/9/30.
//

#pragma once

#include "spdlog/logger.h"
#include <memory>

namespace infinity {

extern std::shared_ptr<spdlog::logger> infinity_logger;

class Logger {
public:
    static void
    Initialize();

    static void
    Shutdown();

};

#define LOG_TRACE(...) infinity::infinity_logger->trace(__VA_ARGS__);
#define LOG_DEBUG(...) infinity::infinity_logger->debug(__VA_ARGS__);
#define LOG_INFO(...) infinity::infinity_logger->info(__VA_ARGS__);
#define LOG_WARN(...) infinity::infinity_logger->warn(__VA_ARGS__);
#define LOG_ERROR(...) infinity::infinity_logger->error(__VA_ARGS__);

}