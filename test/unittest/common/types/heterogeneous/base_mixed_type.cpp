//
// Created by JinHai on 2022/11/23.
//

#include "base_test.h"
#include "common/types/value.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include <gtest/gtest.h>

class BaseMixedTypeTest : public BaseTest {
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

TEST_F(BaseMixedTypeTest, GetTypeName) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kInvalid).c_str(), "Invalid");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kInteger).c_str(), "Integer");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kLongStr).c_str(), "Long String");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kShortStr).c_str(), "Short String");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kTuple).c_str(), "Tuple");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kArray).c_str(), "Array");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNull).c_str(), "Null");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kMissing).c_str(), "Missing");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kDummy).c_str(), "Dummy");
}