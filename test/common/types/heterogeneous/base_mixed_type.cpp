//
// Created by JinHai on 2022/11/23.
//


#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"

class BaseMixedTypeTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
    }
};

TEST_F(BaseMixedTypeTest, GetTypeName) {
    using namespace infinity;

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