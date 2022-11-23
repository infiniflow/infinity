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
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedMissing).c_str(), "Nested Missing");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedInteger).c_str(), "Nested Integer");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedFloat).c_str(), "Nested Float");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedLongStr).c_str(), "Nested Long String");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedShortStr).c_str(), "Nested Short String");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedTuple).c_str(), "Nested Tuple");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedArray).c_str(), "Nested Array");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNestedNull).c_str(), "Nested Null");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kDummy).c_str(), "Dummy");
}