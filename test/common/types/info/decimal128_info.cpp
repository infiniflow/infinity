//
// Created by JinHai on 2022/11/29.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/decimal128_info.h"

class Decimal128InfoTest : public BaseTest {
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

TEST_F(Decimal128InfoTest, decimal_info_A) {
    using namespace infinity;

    EXPECT_THROW(Decimal128Info::Make(39, 39), TypeException);
    EXPECT_THROW(Decimal128Info::Make(38, 39), TypeException);

    auto decimal128_info = Decimal128Info::Make(38, 38);
    EXPECT_EQ(decimal128_info->scale(), 38);
    EXPECT_EQ(decimal128_info->precision(), 38);
    EXPECT_EQ(decimal128_info->Size(), 16);
}
