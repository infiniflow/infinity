//
// Created by JinHai on 2022/9/30.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/logger.h"

class LoggerTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
    }
};

TEST_F(LoggerTest, test1) {
//    infinity::Logger::Initialize();

    LOG_TRACE("This is a trace log");
    LOG_DEBUG("This is a debug log");
    LOG_INFO("This is a info log");
    LOG_WARN("This is a warn log");
    LOG_ERROR("This is a error log");

//    infinity::Logger::Shutdown();
}