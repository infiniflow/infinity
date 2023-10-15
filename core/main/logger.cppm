//
// Created by jinhai on 23-10-15.
//
module;

#include "spdlog/logger.h"
#include <memory>

export module logger;

//import config;
//import third_party;
//import stl;

namespace infinity {

export extern std::shared_ptr<spdlog::logger> infinity_logger;

class Config;

export class Logger {
public:
    static void
    Initialize(const Config* config_ptr);

    static void
    Shutdown();
};

}
