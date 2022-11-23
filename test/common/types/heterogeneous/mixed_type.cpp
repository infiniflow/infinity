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
    MixedType mixed_integer1 = MixedType::MakeInteger(10);
    EXPECT_EQ(mixed_integer1.type, MixedValueType::kInteger);

    auto* integer_mixed_ptr = (IntegerMixedType*)(&mixed_integer1);
    EXPECT_EQ(integer_mixed_ptr->value, 10);

    // Copy constructor
    MixedType mixed_integer2 = mixed_integer1;
    EXPECT_EQ(mixed_integer2.type, MixedValueType::kInteger);
    auto* integer_mixed_ptr2 = (IntegerMixedType*)(&mixed_integer2);

    integer_mixed_ptr->value = 11;
    EXPECT_EQ(integer_mixed_ptr->value, 11);
    EXPECT_EQ(integer_mixed_ptr2->value, 10);

    // Move constructor
    MixedType mixed_integer3 = std::move(mixed_integer2);
    EXPECT_EQ(mixed_integer3.type, MixedValueType::kInteger);
    EXPECT_EQ(mixed_integer2.type, MixedValueType::kInvalid);
    auto* integer_mixed_ptr3 = (IntegerMixedType*)(&mixed_integer3);
    EXPECT_EQ(integer_mixed_ptr3->value, 10);

    // Copy assignment
    MixedType mixed_integer4;
    mixed_integer4 = mixed_integer1;
    EXPECT_EQ(mixed_integer4.type, MixedValueType::kInteger);
    auto* integer_mixed_ptr4 = (IntegerMixedType*)(&mixed_integer4);
    EXPECT_EQ(integer_mixed_ptr4->value, 11);

    // Move assignment
    MixedType mixed_integer5;
    mixed_integer5 = std::move(mixed_integer3);
    EXPECT_EQ(mixed_integer5.type, MixedValueType::kInteger);
    auto* integer_mixed_ptr5 = (IntegerMixedType*)(&mixed_integer5);
    EXPECT_EQ(integer_mixed_ptr5->value, 10);
    EXPECT_EQ(mixed_integer3.type, MixedValueType::kInvalid);
}

TEST_F(MixedTypeTest, mixed_float) {
    using namespace infinity;
    MixedType mixed_float1 = MixedType::MakeFloat(1.005);
    EXPECT_EQ(mixed_float1.type, MixedValueType::kFloat);

    auto* float_mixed_ptr = (FloatMixedType*)(&mixed_float1);
    EXPECT_FLOAT_EQ(float_mixed_ptr->value, 1.005);


    // Copy constructor
    MixedType mixed_float2 = mixed_float1;
    EXPECT_EQ(mixed_float2.type, MixedValueType::kFloat);
    auto* float_mixed_ptr2 = (FloatMixedType*)(&mixed_float2);

    float_mixed_ptr->value = 2.005;
    EXPECT_FLOAT_EQ(float_mixed_ptr->value, 2.005);
    EXPECT_FLOAT_EQ(float_mixed_ptr2->value, 1.005);

    // Move constructor
    MixedType mixed_float3 = std::move(mixed_float2);
    EXPECT_EQ(mixed_float3.type, MixedValueType::kFloat);
    EXPECT_EQ(mixed_float2.type, MixedValueType::kInvalid);
    auto* float_mixed_ptr3 = (FloatMixedType*)(&mixed_float3);
    EXPECT_FLOAT_EQ(float_mixed_ptr3->value, 1.005);

    // Copy assignment
    MixedType mixed_float4;
    mixed_float4 = mixed_float1;
    EXPECT_EQ(mixed_float4.type, MixedValueType::kFloat);
    auto* float_mixed_ptr4 = (FloatMixedType*)(&mixed_float4);
    EXPECT_FLOAT_EQ(float_mixed_ptr4->value, 2.005);

    // Move assignment
    MixedType mixed_float5;
    mixed_float5 = std::move(mixed_float3);
    EXPECT_EQ(mixed_float5.type, MixedValueType::kFloat);
    auto* float_mixed_ptr5 = (FloatMixedType*)(&mixed_float5);
    EXPECT_FLOAT_EQ(float_mixed_ptr5->value, 1.005);
    EXPECT_EQ(mixed_float3.type, MixedValueType::kInvalid);
}

TEST_F(MixedTypeTest, mixed_short_str) {
    using namespace infinity;

    // Short string
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
        result.clear();

        // Copy constructor
        MixedType mixed_short_string2 = mixed_short_string;
        EXPECT_EQ(mixed_short_string2.type, MixedValueType::kShortStr);
        auto *short_string_mixed_ptr2 = (ShortStrMixedType *) (&mixed_short_string2);
        result = String(short_string_mixed_ptr2->ptr, short_string_mixed_ptr2->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr2->length, 14);

        // validate mixed_short_string not modified
        result = String(short_string_mixed_ptr->ptr, short_string_mixed_ptr->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr->length, 14);

        // Move constructor
        MixedType mixed_short_string3 = std::move(mixed_short_string2);
        EXPECT_EQ(mixed_short_string3.type, MixedValueType::kShortStr);
        auto *short_string_mixed_ptr3 = (ShortStrMixedType *) (&mixed_short_string3);
        result = String(short_string_mixed_ptr3->ptr, short_string_mixed_ptr3->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr3->length, 14);

        // validate mixed_short_string2 is reset
        EXPECT_EQ(short_string_mixed_ptr2->type, MixedValueType::kInvalid);
        EXPECT_STREQ(short_string_mixed_ptr2->ptr, "");
        EXPECT_EQ(short_string_mixed_ptr2->length, 0);

        // Copy assignment
        MixedType mixed_short_string4;
        mixed_short_string4 = mixed_short_string3;
        EXPECT_EQ(mixed_short_string4.type, MixedValueType::kShortStr);
        auto *short_string_mixed_ptr4 = (ShortStrMixedType *) (&mixed_short_string4);
        result = String(short_string_mixed_ptr4->ptr, short_string_mixed_ptr4->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr4->length, 14);

        // validate mixed_short_string3 not modified
        result = String(short_string_mixed_ptr3->ptr, short_string_mixed_ptr3->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr3->length, 14);

        // Move assignment
        MixedType mixed_short_string5;
        mixed_short_string5 = std::move(mixed_short_string3);
        EXPECT_EQ(mixed_short_string5.type, MixedValueType::kShortStr);
        auto *short_string_mixed_ptr5 = (ShortStrMixedType *) (&mixed_short_string5);
        result = String(short_string_mixed_ptr5->ptr, short_string_mixed_ptr5->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr5->length, 14);

        // validate mixed_short_string3 is reset
        EXPECT_EQ(short_string_mixed_ptr3->type, MixedValueType::kInvalid);
        EXPECT_STREQ(short_string_mixed_ptr3->ptr, "");
        EXPECT_EQ(short_string_mixed_ptr3->length, 0);
    }


}

TEST_F(MixedTypeTest, mixed_long_str) {
    using namespace infinity;

    // Long string
    MixedType mixed_long_string = MixedType::MakeString("Hello the world");
    EXPECT_EQ(mixed_long_string.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr = (LongStrMixedType *) (&mixed_long_string);
    EXPECT_STREQ(long_string_mixed_ptr->ptr, "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr->length, 15);
    String header = String(long_string_mixed_ptr->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header.c_str(), "Hello");

    // Copy constructor
    MixedType mixed_long_string1 = mixed_long_string;
    EXPECT_EQ(mixed_long_string1.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr1 = (LongStrMixedType *) (&mixed_long_string1);
    EXPECT_STREQ(long_string_mixed_ptr1->ptr, "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr1->length, 15);
    String header1 = String(long_string_mixed_ptr1->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header1.c_str(), "Hello");

    // Reset mixed_long_string
    long_string_mixed_ptr->Reset();
    EXPECT_EQ(long_string_mixed_ptr->ptr, nullptr);
    EXPECT_EQ(long_string_mixed_ptr->length, 0);

    // Move constructor
    MixedType mixed_long_string2 = std::move(mixed_long_string1);
    EXPECT_EQ(mixed_long_string2.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr2 = (LongStrMixedType *) (&mixed_long_string2);
    EXPECT_STREQ(long_string_mixed_ptr2->ptr, "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr2->length, 15);
    String header2 = String(long_string_mixed_ptr2->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header2.c_str(), "Hello");

    // Validate mixed_long_string1 is reset.
    EXPECT_EQ(long_string_mixed_ptr1->ptr, nullptr);
    EXPECT_EQ(long_string_mixed_ptr1->length, 0);

    // Copy assignment
    MixedType mixed_long_string3;
    mixed_long_string3 = mixed_long_string2;
    EXPECT_EQ(mixed_long_string3.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr3 = (LongStrMixedType *) (&mixed_long_string3);
    EXPECT_STREQ(long_string_mixed_ptr3->ptr, "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr3->length, 15);
    String header3 = String(long_string_mixed_ptr3->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header3.c_str(), "Hello");

    // Move assignment
    MixedType mixed_long_string4;
    mixed_long_string4 = std::move(mixed_long_string2);
    EXPECT_EQ(mixed_long_string4.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr4 = (LongStrMixedType *) (&mixed_long_string4);

    EXPECT_STREQ(long_string_mixed_ptr4->ptr, "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr4->length, 15);
    String header4 = String(long_string_mixed_ptr4->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header4.c_str(), "Hello");

    // Validate mixed_long_string2 is reset.
    EXPECT_EQ(long_string_mixed_ptr2->ptr, nullptr);
    EXPECT_EQ(long_string_mixed_ptr2->length, 0);
}

TEST_F(MixedTypeTest, mixed_tuple) {
    using namespace infinity;


}
