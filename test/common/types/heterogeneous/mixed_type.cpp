//
// Created by JinHai on 2022/11/21.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"

class MixedTypeTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
    }
};

TEST_F(MixedTypeTest, mixed_integer) {
    using namespace infinity;
    MixedType mixed_integer = MixedType::MakeInteger(10);
    EXPECT_EQ(mixed_integer.type, MixedValueType::kInteger);

    auto* integer_mixed_ptr = (IntegerMixedType*)(&mixed_integer);
    EXPECT_EQ(integer_mixed_ptr->value, 10);
}

TEST_F(MixedTypeTest, mixed_float) {
    using namespace infinity;
    MixedType mixed_float = MixedType::MakeFloat(1.005);
    EXPECT_EQ(mixed_float.type, MixedValueType::kFloat);

    auto* float_mixed_ptr = (FloatMixedType*)(&mixed_float);
    EXPECT_FLOAT_EQ(float_mixed_ptr->value, 1.005);
}

TEST_F(MixedTypeTest, mixed_string) {
    using namespace infinity;
    {
        MixedType mixed_short_string = MixedType::MakeString("Hello");
        EXPECT_EQ(mixed_short_string.type, MixedValueType::kShortStr);

        auto *short_string_mixed_ptr = (ShortStrMixedType *) (&mixed_short_string);
        String result = String(short_string_mixed_ptr->ptr, short_string_mixed_ptr->length);
        EXPECT_STREQ(result.c_str(), "Hello");
        EXPECT_EQ(short_string_mixed_ptr->length, 5);

        mixed_short_string.Reset();

        mixed_short_string = MixedType::MakeString("Hello a World!");
        EXPECT_EQ(mixed_short_string.type, MixedValueType::kShortStr);
        result = String(short_string_mixed_ptr->ptr, short_string_mixed_ptr->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr->length, 14);
    }

    {
        MixedType mixed_long_string = MixedType::MakeString("Hello the world");
        EXPECT_EQ(mixed_long_string.type, MixedValueType::kLongStr);
        auto *long_string_mixed_ptr = (LongStrMixedType *) (&mixed_long_string);
        EXPECT_STREQ(long_string_mixed_ptr->ptr, "Hello the world");
        EXPECT_EQ(long_string_mixed_ptr->length, 15);
        String header = String(long_string_mixed_ptr->ptr, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(header.c_str(), "Hello");
        long_string_mixed_ptr->Reset();
    }
}
