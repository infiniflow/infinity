//
// Created by JinHai on 2022/9/30.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class LoggerTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(LoggerTest, test1) {
    LOG_TRACE("This is a trace log");
    LOG_DEBUG("This is a debug log");
    LOG_INFO("This is a info log");
    LOG_WARN("This is a warn log");
    LOG_ERROR("This is a error log");
}