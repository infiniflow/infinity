//
// Created by jinhai on 23-3-10.
//


#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class DateTypeTest : public BaseTest {
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

TEST_F(DateTypeTest, test1) {
    using namespace infinity;

    DateType date1;

    EXPECT_EQ(date1.ToString(), "1970-01-01");

}