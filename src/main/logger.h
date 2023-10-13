//
// Created by JinHai on 2022/9/30.
//

#pragma once

#include "config.h"
#include "spdlog/logger.h"

namespace infinity {

extern SharedPtr<spdlog::logger> infinity_logger;

class Logger {
public:
    static void Initialize(const Config *config_ptr);

    static void Shutdown();
};

#define LOG_TRACE(...) infinity::infinity_logger->trace(__VA_ARGS__);
#define LOG_INFO(...) infinity::infinity_logger->info(__VA_ARGS__);
#define LOG_WARN(...) infinity::infinity_logger->warn(__VA_ARGS__);
#define LOG_ERROR(...) infinity::infinity_logger->error(__VA_ARGS__);
#define LOG_CRITICAL(...) infinity::infinity_logger->critical(__VA_ARGS__);

} // namespace infinity