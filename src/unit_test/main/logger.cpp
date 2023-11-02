//
// Created by JinHai on 2022/9/30.
//

#include "unit_test/base_test.h"

import infinity_context;
import global_resource_usage;
import logger;

class LoggerTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        system("rm -rf /tmp/infinity/log /tmp/infinity/data /tmp/infinity/wal");
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity/log /tmp/infinity/data /tmp/infinity/wal");
        BaseTest::TearDown();
    }
};
TEST_F(LoggerTest, test1) {
    using namespace infinity;
    LOG_TRACE("This is a trace log");
    LOG_INFO("This is a info log");
    LOG_WARN("This is a warn log");
    LOG_ERROR("This is a error log");
    LOG_CRITICAL("This is a error log");
}