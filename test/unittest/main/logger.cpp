//
// Created by JinHai on 2022/9/30.
//

#include "main/logger.h"
#include "base_test.h"
#include "main/infinity.h"
#include "main/stats/global_resource_usage.h"
#include <gtest/gtest.h>

class LoggerTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(LoggerTest, test1) {
    LOG_TRACE("This is a trace log");
    LOG_INFO("This is a info log");
    LOG_WARN("This is a warn log");
    LOG_ERROR("This is a error log");
    LOG_CRITICAL("This is a error log");
}