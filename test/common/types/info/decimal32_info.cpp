//
// Created by JinHai on 2022/11/29.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/decimal32_info.h"

class Decimal32InfoTest : public BaseTest {
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

TEST_F(Decimal32InfoTest, decimal_info_A) {
    using namespace infinity;

    EXPECT_THROW(Decimal32Info::Make(10, 10), TypeException);
    EXPECT_THROW(Decimal32Info::Make(9, 10), TypeException);

    auto decimal32_info = Decimal32Info::Make(9, 9);
    EXPECT_EQ(decimal32_info->scale(), 9);
    EXPECT_EQ(decimal32_info->precision(), 9);
    EXPECT_EQ(decimal32_info->Size(), 4);
}
