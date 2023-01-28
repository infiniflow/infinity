//
// Created by JinHai on 2022/9/30.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class LoggerTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
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