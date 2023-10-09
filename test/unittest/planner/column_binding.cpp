//
// Created by jinhai on 23-2-17.
//

#include "planner/column_binding.h"
#include "base_test.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/profiler/base_profiler.h"
#include "main/stats/global_resource_usage.h"
#include <gtest/gtest.h>

class ColumnBindingTest : public BaseTest {
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

TEST_F(ColumnBindingTest, test1) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    ColumnBinding column1(0, 1);
    ColumnBinding column2(0, 1);
    ColumnBinding column3(1, 2);

    EXPECT_TRUE(column1 == column2);
    EXPECT_TRUE(column1 != column3);
}