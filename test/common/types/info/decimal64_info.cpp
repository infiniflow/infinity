//
// Created by JinHai on 2022/11/29.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/decimal64_info.h"

class Decimal64InfoTest : public BaseTest {
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

TEST_F(Decimal64InfoTest, decimal_info_A) {
    using namespace infinity;

    EXPECT_THROW(Decimal64Info::Make(19, 19), TypeException);
    EXPECT_THROW(Decimal64Info::Make(18, 19), TypeException);

    auto decimal64_info = Decimal64Info::Make(18, 18);
    EXPECT_EQ(decimal64_info->scale(), 18);
    EXPECT_EQ(decimal64_info->precision(), 18);
    EXPECT_EQ(decimal64_info->Size(), 8);
}
