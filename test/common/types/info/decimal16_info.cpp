//
// Created by JinHai on 2022/11/29.
//



#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/decimal16_info.h"

class Decimal16InfoTest : public BaseTest {
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

TEST_F(Decimal16InfoTest, decimal_info_A) {
    using namespace infinity;

    EXPECT_THROW(Decimal16Info::Make(5, 5), TypeException);
    EXPECT_THROW(Decimal16Info::Make(4, 5), TypeException);

    auto decimal16_info = Decimal16Info::Make(4, 4);
    EXPECT_EQ(decimal16_info->scale(), 4);
    EXPECT_EQ(decimal16_info->precision(), 4);
    EXPECT_EQ(decimal16_info->Size(), 2);
}
