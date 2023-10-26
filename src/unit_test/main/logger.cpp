//
// Created by JinHai on 2022/9/30.
//

#include "unit_test/base_test.h"

import infinity;
import global_resource_usage;
import logger;

class LoggerTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
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