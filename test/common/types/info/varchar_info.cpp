//
// Created by JinHai on 2022/11/27.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"

class VarcharInfoTest : public BaseTest {
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

TEST_F(VarcharInfoTest, varchar_info_A) {
    using namespace infinity;

    EXPECT_THROW(VarcharInfo::Make(64), TypeException);
    EXPECT_THROW(VarcharInfo::Make(65536), TypeException);

    auto varchar_info_ptr = VarcharInfo::Make(65);
    EXPECT_EQ(varchar_info_ptr->length_limit(), 65);
    EXPECT_EQ(varchar_info_ptr->Size(), 16);

    varchar_info_ptr = VarcharInfo::Make();
    EXPECT_EQ(varchar_info_ptr->length_limit(), MAX_VARCHAR_SIZE);
    EXPECT_EQ(varchar_info_ptr->Size(), 16);

}