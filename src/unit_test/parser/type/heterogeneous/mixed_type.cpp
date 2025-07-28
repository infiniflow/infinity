// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gtest/gtest.h"
import base_test;

import infinity_exception;

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;
import internal_types;
import parser_assert;

using namespace infinity;
class MixedTypeTest : public BaseTest {};

TEST_F(MixedTypeTest, mixed_integer) {
    using namespace infinity;

    MixedType mixed_integer1 = MixedType::MakeInteger(10);
    EXPECT_EQ(mixed_integer1.type, MixedValueType::kInteger);

    auto *integer_mixed_ptr = (IntegerMixedType *)(&mixed_integer1);
    EXPECT_EQ(integer_mixed_ptr->value, 10);

    // Copy constructor
    MixedType mixed_integer2 = mixed_integer1;
    EXPECT_EQ(mixed_integer2.type, MixedValueType::kInteger);
    auto *integer_mixed_ptr2 = (IntegerMixedType *)(&mixed_integer2);

    integer_mixed_ptr->value = 11;
    EXPECT_EQ(integer_mixed_ptr->value, 11);
    EXPECT_EQ(integer_mixed_ptr2->value, 10);

    // std::move constructor
    MixedType mixed_integer3 = std::move(mixed_integer2);
    EXPECT_EQ(mixed_integer3.type, MixedValueType::kInteger);
    EXPECT_EQ(mixed_integer2.type, MixedValueType::kInvalid);
    auto *integer_mixed_ptr3 = (IntegerMixedType *)(&mixed_integer3);
    EXPECT_EQ(integer_mixed_ptr3->value, 10);

    // Copy assignment
    MixedType mixed_integer4;
    mixed_integer4 = mixed_integer1;
    EXPECT_EQ(mixed_integer4.type, MixedValueType::kInteger);
    auto *integer_mixed_ptr4 = (IntegerMixedType *)(&mixed_integer4);
    EXPECT_EQ(integer_mixed_ptr4->value, 11);

    // std::move assignment
    MixedType mixed_integer5;
    mixed_integer5 = std::move(mixed_integer3);
    EXPECT_EQ(mixed_integer5.type, MixedValueType::kInteger);
    auto *integer_mixed_ptr5 = (IntegerMixedType *)(&mixed_integer5);
    EXPECT_EQ(integer_mixed_ptr5->value, 10);
    EXPECT_EQ(mixed_integer3.type, MixedValueType::kInvalid);
}

TEST_F(MixedTypeTest, mixed_float) {
    using namespace infinity;

    MixedType mixed_float1 = MixedType::MakeFloat(1.005);
    EXPECT_EQ(mixed_float1.type, MixedValueType::kFloat);

    auto *float_mixed_ptr = (FloatMixedType *)(&mixed_float1);
    EXPECT_FLOAT_EQ(float_mixed_ptr->value, 1.005);

    // Copy constructor
    MixedType mixed_float2 = mixed_float1;
    EXPECT_EQ(mixed_float2.type, MixedValueType::kFloat);
    auto *float_mixed_ptr2 = (FloatMixedType *)(&mixed_float2);

    float_mixed_ptr->value = 2.005;
    EXPECT_FLOAT_EQ(float_mixed_ptr->value, 2.005);
    EXPECT_FLOAT_EQ(float_mixed_ptr2->value, 1.005);

    // std::move constructor
    MixedType mixed_float3 = std::move(mixed_float2);
    EXPECT_EQ(mixed_float3.type, MixedValueType::kFloat);
    EXPECT_EQ(mixed_float2.type, MixedValueType::kInvalid);
    auto *float_mixed_ptr3 = (FloatMixedType *)(&mixed_float3);
    EXPECT_FLOAT_EQ(float_mixed_ptr3->value, 1.005);

    // Copy assignment
    MixedType mixed_float4;
    mixed_float4 = mixed_float1;
    EXPECT_EQ(mixed_float4.type, MixedValueType::kFloat);
    auto *float_mixed_ptr4 = (FloatMixedType *)(&mixed_float4);
    EXPECT_FLOAT_EQ(float_mixed_ptr4->value, 2.005);

    // std::move assignment
    MixedType mixed_float5;
    mixed_float5 = std::move(mixed_float3);
    EXPECT_EQ(mixed_float5.type, MixedValueType::kFloat);
    auto *float_mixed_ptr5 = (FloatMixedType *)(&mixed_float5);
    EXPECT_FLOAT_EQ(float_mixed_ptr5->value, 1.005);
    EXPECT_EQ(mixed_float3.type, MixedValueType::kInvalid);
}

TEST_F(MixedTypeTest, mixed_short_str) {
    using namespace infinity;

    // Short string
    {
        MixedType mixed_short_string = MixedType::MakeString("Hello");
        EXPECT_EQ(mixed_short_string.type, MixedValueType::kShortStr);

        auto *short_string_mixed_ptr = (ShortStrMixedType *)(&mixed_short_string);
        String result = String(short_string_mixed_ptr->ptr, short_string_mixed_ptr->length);
        EXPECT_STREQ(result.c_str(), "Hello");
        EXPECT_EQ(short_string_mixed_ptr->length, 5);
        String another = "Hell";
        EXPECT_TRUE(short_string_mixed_ptr->Equal(result));
        EXPECT_FALSE(short_string_mixed_ptr->Equal(another));

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
        auto *short_string_mixed_ptr2 = (ShortStrMixedType *)(&mixed_short_string2);
        result = String(short_string_mixed_ptr2->ptr, short_string_mixed_ptr2->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr2->length, 14);

        // validate mixed_short_string not modified
        result = String(short_string_mixed_ptr->ptr, short_string_mixed_ptr->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr->length, 14);

        // std::move constructor
        MixedType mixed_short_string3 = std::move(mixed_short_string2);
        EXPECT_EQ(mixed_short_string3.type, MixedValueType::kShortStr);
        auto *short_string_mixed_ptr3 = (ShortStrMixedType *)(&mixed_short_string3);
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
        auto *short_string_mixed_ptr4 = (ShortStrMixedType *)(&mixed_short_string4);
        result = String(short_string_mixed_ptr4->ptr, short_string_mixed_ptr4->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr4->length, 14);

        // validate mixed_short_string3 not modified
        result = String(short_string_mixed_ptr3->ptr, short_string_mixed_ptr3->length);
        EXPECT_STREQ(result.c_str(), "Hello a World!");
        EXPECT_EQ(short_string_mixed_ptr3->length, 14);

        // std::move assignment
        MixedType mixed_short_string5;
        mixed_short_string5 = std::move(mixed_short_string3);
        EXPECT_EQ(mixed_short_string5.type, MixedValueType::kShortStr);
        auto *short_string_mixed_ptr5 = (ShortStrMixedType *)(&mixed_short_string5);
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
    auto *long_string_mixed_ptr = (LongStrMixedType *)(&mixed_long_string);

    String long_str(long_string_mixed_ptr->ptr, long_string_mixed_ptr->length);
    EXPECT_STREQ(long_str.c_str(), "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr->length, 15);
    String header = String(long_string_mixed_ptr->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header.c_str(), "Hello");

    String another = "Aello the world";
    EXPECT_FALSE(long_string_mixed_ptr->Equal(another));
    another = "Hellow";
    EXPECT_FALSE(long_string_mixed_ptr->Equal(another));
    another = "Hello the world";
    EXPECT_TRUE(long_string_mixed_ptr->Equal(another));

    // Copy constructor
    MixedType mixed_long_string1 = mixed_long_string;
    EXPECT_EQ(mixed_long_string1.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr1 = (LongStrMixedType *)(&mixed_long_string1);
    long_str = String(long_string_mixed_ptr->ptr, long_string_mixed_ptr->length);
    EXPECT_STREQ(long_str.c_str(), "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr1->length, 15);
    String header1 = String(long_string_mixed_ptr1->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header1.c_str(), "Hello");

    // Reset mixed_long_string
    long_string_mixed_ptr->Reset();
    EXPECT_EQ(long_string_mixed_ptr->ptr, nullptr);
    EXPECT_EQ(long_string_mixed_ptr->length, 0);

    // std::move constructor
    MixedType mixed_long_string2 = std::move(mixed_long_string1);
    EXPECT_EQ(mixed_long_string2.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr2 = (LongStrMixedType *)(&mixed_long_string2);
    long_str = String(long_string_mixed_ptr2->ptr, long_string_mixed_ptr2->length);
    EXPECT_STREQ(long_str.c_str(), "Hello the world");
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
    auto *long_string_mixed_ptr3 = (LongStrMixedType *)(&mixed_long_string3);
    long_str = String(long_string_mixed_ptr3->ptr, long_string_mixed_ptr3->length);
    EXPECT_STREQ(long_str.c_str(), "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr3->length, 15);
    String header3 = String(long_string_mixed_ptr3->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header3.c_str(), "Hello");

    // std::move assignment
    MixedType mixed_long_string4;
    mixed_long_string4 = std::move(mixed_long_string2);
    EXPECT_EQ(mixed_long_string4.type, MixedValueType::kLongStr);
    auto *long_string_mixed_ptr4 = (LongStrMixedType *)(&mixed_long_string4);
    long_str = String(long_string_mixed_ptr4->ptr, long_string_mixed_ptr4->length);
    EXPECT_STREQ(long_str.c_str(), "Hello the world");
    EXPECT_EQ(long_string_mixed_ptr4->length, 15);
    String header4 = String(long_string_mixed_ptr4->ptr, BaseMixedType::LONG_STR_HEADER);
    EXPECT_STREQ(header4.c_str(), "Hello");

    // Validate mixed_long_string2 is reset.
    EXPECT_EQ(long_string_mixed_ptr2->ptr, nullptr);
    EXPECT_EQ(long_string_mixed_ptr2->length, 0);
}

TEST_F(MixedTypeTest, mixed_null) {
    using namespace infinity;

    MixedType mixed_null1 = MixedType::MakeNull();
    EXPECT_EQ(mixed_null1.type, MixedValueType::kNull);
}

TEST_F(MixedTypeTest, mixed_miss) {
    using namespace infinity;

    MixedType mixed_miss1 = MixedType::MakeMissing();
    EXPECT_EQ(mixed_miss1.type, MixedValueType::kMissing);
}

TEST_F(MixedTypeTest, mixed_tuple1) {
    using namespace infinity;

    MixedType mixed_tuple1 = MixedType::MakeTuple(7);
    EXPECT_EQ(mixed_tuple1.type, MixedValueType::kTuple);
    {
        auto *tuple_ptr1 = (TupleMixedType *)(&mixed_tuple1);
        EXPECT_EQ(tuple_ptr1->count, 7);
    }

    // Exception
    EXPECT_THROW(mixed_tuple1.InsertIntegerIntoArray(100, 10), ParserException);

    // Key1: integer
    mixed_tuple1.InsertIntegerIntoTuple("key1", 100);
    {
        auto *value1_ptr = mixed_tuple1.GetFromTuple("key1");
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 100);
    }

    // Key2: float
    mixed_tuple1.InsertFloatIntoTuple("key2", 1.1);
    {
        auto *value2_ptr = mixed_tuple1.GetFromTuple("key2");
        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 1.1);
    }

    // Key3: short string
    mixed_tuple1.InsertStringIntoTuple("key3", "Hello World !!");
    {
        auto *value3_ptr = mixed_tuple1.GetFromTuple("key3");
        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

        String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "Hello World !!");
    }

    // Key4: long string
    mixed_tuple1.InsertStringIntoTuple("key4", "Hello World Hello World");
    {
        auto *value4_ptr = mixed_tuple1.GetFromTuple("key4");
        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

        String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "Hello World Hello World");
    }

    // Key5: null
    mixed_tuple1.InsertNullIntoTuple("key5");
    {
        auto *value5_ptr = mixed_tuple1.GetFromTuple("key5");
        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
    }

    // Key6: tuple with 5 trivial type inside
    {
        MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
        {
            EXPECT_EQ(mixed_tuple_key6.type, MixedValueType::kTuple);
            auto *tuple_key6_ptr = (TupleMixedType *)(&mixed_tuple_key6);
            EXPECT_EQ(tuple_key6_ptr->count, 5);
        }

        // Key61: integer
        mixed_tuple_key6.InsertIntegerIntoTuple("key61", 200);
        {
            auto *value61_ptr = mixed_tuple_key6.GetFromTuple("key61");
            EXPECT_EQ(value61_ptr->type, MixedValueType::kInteger);
            auto *integer_value61_ptr = (IntegerMixedType *)(value61_ptr);
            EXPECT_EQ(integer_value61_ptr->value, 200);
        }

        // Key62: float
        mixed_tuple_key6.InsertFloatIntoTuple("key62", 6.1);
        {
            auto *value62_ptr = mixed_tuple_key6.GetFromTuple("key62");
            EXPECT_EQ(value62_ptr->type, MixedValueType::kFloat);
            auto *float_value62_ptr = (FloatMixedType *)(value62_ptr);
            EXPECT_FLOAT_EQ(float_value62_ptr->value, 6.1);
        }

        // Key63: short string
        mixed_tuple_key6.InsertStringIntoTuple("key63", "Hello World !!");
        {
            auto *value63_ptr = mixed_tuple_key6.GetFromTuple("key63");
            EXPECT_EQ(value63_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value63_ptr = (ShortStrMixedType *)(value63_ptr);

            String result(short_str_value63_ptr->ptr, short_str_value63_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World !!");
        }

        // Key64: long string
        mixed_tuple_key6.InsertStringIntoTuple("key64", "Hello World Hello World");
        {
            auto *value64_ptr = mixed_tuple_key6.GetFromTuple("key64");
            EXPECT_EQ(value64_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value64_ptr = (LongStrMixedType *)(value64_ptr);

            String result(long_str_value64_ptr->ptr, long_str_value64_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World Hello World");
        }

        // Key65: null
        mixed_tuple_key6.InsertNullIntoTuple("key65");
        {
            auto *value65_ptr = mixed_tuple_key6.GetFromTuple("key65");
            EXPECT_EQ(value65_ptr->type, MixedValueType::kNull);
        }

        mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);

        {
            //
            // Check the value of mixed_tuple_key6 after copy
            //

            {
                EXPECT_EQ(mixed_tuple_key6.type, MixedValueType::kTuple);
                auto *tuple_key6_ptr = (TupleMixedType *)(&mixed_tuple_key6);
                EXPECT_EQ(tuple_key6_ptr->count, 5);
            }
            // Key61: integer
            {
                auto *value61_ptr = mixed_tuple_key6.GetFromTuple("key61");
                EXPECT_EQ(value61_ptr->type, MixedValueType::kInteger);
                auto *integer_value61_ptr = (IntegerMixedType *)(value61_ptr);
                EXPECT_EQ(integer_value61_ptr->value, 200);
            }
            // Key62: float
            {
                auto *value62_ptr = mixed_tuple_key6.GetFromTuple("key62");
                EXPECT_EQ(value62_ptr->type, MixedValueType::kFloat);
                auto *float_value62_ptr = (FloatMixedType *)(value62_ptr);
                EXPECT_FLOAT_EQ(float_value62_ptr->value, 6.1);
            }
            // Key63: short string
            {
                auto *value63_ptr = mixed_tuple_key6.GetFromTuple("key63");
                EXPECT_EQ(value63_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value63_ptr = (ShortStrMixedType *)(value63_ptr);

                String result(short_str_value63_ptr->ptr, short_str_value63_ptr->length);
                EXPECT_STREQ(result.c_str(), "Hello World !!");
            }
            // Key64: long string
            {
                auto *value64_ptr = mixed_tuple_key6.GetFromTuple("key64");
                EXPECT_EQ(value64_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value64_ptr = (LongStrMixedType *)(value64_ptr);

                String result(long_str_value64_ptr->ptr, long_str_value64_ptr->length);
                EXPECT_STREQ(result.c_str(), "Hello World Hello World");
            }
            // Key65: null
            {
                auto *value65_ptr = mixed_tuple_key6.GetFromTuple("key65");
                EXPECT_EQ(value65_ptr->type, MixedValueType::kNull);
            }
        }
    }
    {
        auto *value6_ptr = mixed_tuple1.GetFromTuple("key6");
        EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
    }

    // Key7: array with 5 trivial type inside
    {
        MixedType mixed_array7 = MixedType::MakeArray(5);
        EXPECT_EQ(mixed_array7.type, MixedValueType::kArray);
        {
            auto *array_ptr1 = (ArrayMixedType *)(&mixed_array7);
            EXPECT_EQ(array_ptr1->count, 5);
        }

        // Integer
        mixed_array7.InsertIntegerIntoArray(4300, 0);
        {
            auto *value1_ptr = mixed_array7.GetByIndex(0);
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 4300);
        }

        // Float
        mixed_array7.InsertFloatIntoArray(5.5, 1);
        {
            auto *value2_ptr = mixed_array7.GetByIndex(1);
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 5.5);
        }

        // Short str
        mixed_array7.InsertStringIntoArray("FunnyHalloween", 2);
        {
            auto *value3_ptr = mixed_array7.GetByIndex(2);
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

            String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "FunnyHalloween");
        }

        // Long str
        mixed_array7.InsertStringIntoArray("FunnyHalloween OK", 3);
        {
            auto *value4_ptr = mixed_array7.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

            String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "FunnyHalloween OK");
        }

        // Null
        mixed_array7.InsertNullIntoArray(4);
        {
            auto *value4_ptr = mixed_array7.GetByIndex(4);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
        }

        mixed_tuple1.CopyIntoTuple("key7", mixed_array7);

        {
            //
            // Check the value of mixed_array7 after copy
            //
            EXPECT_EQ(mixed_array7.type, MixedValueType::kArray);
            {
                auto *array_ptr1 = (ArrayMixedType *)(&mixed_array7);
                EXPECT_EQ(array_ptr1->count, 5);
            }
            // Integer
            {
                auto *value1_ptr = mixed_array7.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, 4300);
            }
            // Float
            {
                auto *value2_ptr = mixed_array7.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, 5.5);
            }
            // Short str
            {
                auto *value3_ptr = mixed_array7.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "FunnyHalloween");
            }
            // Long str
            {
                auto *value4_ptr = mixed_array7.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "FunnyHalloween OK");
            }
            // Null
            {
                auto *value4_ptr = mixed_array7.GetByIndex(4);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
            }
        }
    }
    {
        auto *value7_ptr = mixed_tuple1.GetFromTuple("key7");
        EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
    }

    // Recheck all value and nested value in tuple;
    {
        EXPECT_EQ(mixed_tuple1.type, MixedValueType::kTuple);
        {
            auto *tuple_ptr1 = (TupleMixedType *)(&mixed_tuple1);
            EXPECT_EQ(tuple_ptr1->count, 7);
        }
        // Key1: integer
        {
            auto *value1_ptr = mixed_tuple1.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 100);
        }
        // Key2: float
        {
            auto *value2_ptr = mixed_tuple1.GetFromTuple("key2");
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.1);
        }
        // Key3: short string
        {
            auto *value3_ptr = mixed_tuple1.GetFromTuple("key3");
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World !!");
        }
        // Key4: long string
        {
            auto *value4_ptr = mixed_tuple1.GetFromTuple("key4");
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World Hello World");
        }
        // Key5: null
        {
            auto *value5_ptr = mixed_tuple1.GetFromTuple("key5");
            EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
        }
        // Key6: tuple with 5 trivial type inside
        {
            auto *value6_ptr = mixed_tuple1.GetFromTuple("key6");
            {
                EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                auto *tuple_key6_ptr = (TupleMixedType *)(value6_ptr);
                EXPECT_EQ(tuple_key6_ptr->count, 5);
            }
            // Key61: integer
            {
                auto *value61_ptr = value6_ptr->GetFromTuple("key61");
                EXPECT_EQ(value61_ptr->type, MixedValueType::kInteger);
                auto *integer_value61_ptr = (IntegerMixedType *)(value61_ptr);
                EXPECT_EQ(integer_value61_ptr->value, 200);
            }
            // Key62: float
            {
                auto *value62_ptr = value6_ptr->GetFromTuple("key62");
                EXPECT_EQ(value62_ptr->type, MixedValueType::kFloat);
                auto *float_value62_ptr = (FloatMixedType *)(value62_ptr);
                EXPECT_FLOAT_EQ(float_value62_ptr->value, 6.1);
            }
            // Key63: short string
            {
                auto *value63_ptr = value6_ptr->GetFromTuple("key63");
                EXPECT_EQ(value63_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value63_ptr = (ShortStrMixedType *)(value63_ptr);

                String result(short_str_value63_ptr->ptr, short_str_value63_ptr->length);
                EXPECT_STREQ(result.c_str(), "Hello World !!");
            }
            // Key64: long string
            {
                auto *value64_ptr = value6_ptr->GetFromTuple("key64");
                EXPECT_EQ(value64_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value64_ptr = (LongStrMixedType *)(value64_ptr);

                String result(long_str_value64_ptr->ptr, long_str_value64_ptr->length);
                EXPECT_STREQ(result.c_str(), "Hello World Hello World");
            }
            // Key65: null
            {
                auto *value65_ptr = value6_ptr->GetFromTuple("key65");
                EXPECT_EQ(value65_ptr->type, MixedValueType::kNull);
            }
        }
        // Key7: array with 5 trivial type inside
        {
            auto *value7_ptr = mixed_tuple1.GetFromTuple("key7");
            EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
            {
                auto *array_ptr1 = (ArrayMixedType *)(value7_ptr);
                EXPECT_EQ(array_ptr1->count, 5);
            }
            // Integer
            {
                auto *value1_ptr = value7_ptr->GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, 4300);
            }
            // Float
            {
                auto *value2_ptr = value7_ptr->GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, 5.5);
            }
            // Short str
            {
                auto *value3_ptr = value7_ptr->GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "FunnyHalloween");
            }
            // Long str
            {
                auto *value4_ptr = value7_ptr->GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "FunnyHalloween OK");
            }
            // Null
            {
                auto *value4_ptr = value7_ptr->GetByIndex(4);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
            }
        }
    }
}

TEST_F(MixedTypeTest, mixed_tuple2) {
    using namespace infinity;

    MixedType mixed_tuple1 = MixedType::MakeTuple(7);
    EXPECT_EQ(mixed_tuple1.type, MixedValueType::kTuple);
    auto *tuple_ptr1 = (TupleMixedType *)(&mixed_tuple1);
    EXPECT_EQ(tuple_ptr1->count, 7);

    // Exception
    EXPECT_THROW(mixed_tuple1.InsertIntegerIntoArray(100, 10), ParserException);

    // Key1: integer
    mixed_tuple1.InsertIntegerIntoTuple("key1", 100);
    {
        auto *value1_ptr = mixed_tuple1.GetFromTuple("key1");
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 100);
    }

    // Key2: float
    mixed_tuple1.InsertFloatIntoTuple("key2", 1.1);
    {
        auto *value2_ptr = mixed_tuple1.GetFromTuple("key2");
        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 1.1);
    }

    // Key3: short string
    mixed_tuple1.InsertStringIntoTuple("key3", "Hello World !!");
    {
        auto *value3_ptr = mixed_tuple1.GetFromTuple("key3");
        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);
        String short_str(short_str_value_ptr->ptr, short_str_value_ptr->length);
        EXPECT_STREQ(short_str.c_str(), "Hello World !!");
    }

    // Key4: long string
    mixed_tuple1.InsertStringIntoTuple("key4", "Hello World Hello World");
    {
        auto *value4_ptr = mixed_tuple1.GetFromTuple("key4");
        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);
        String long_str(long_str_value_ptr->ptr, long_str_value_ptr->length);
        EXPECT_STREQ(long_str.c_str(), "Hello World Hello World");
    }

    // Key5: null
    mixed_tuple1.InsertNullIntoTuple("key5");
    {
        auto *value5_ptr = mixed_tuple1.GetFromTuple("key5");
        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
    }

    // Key6: tuple with 5 trivial type inside
    {
        MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
        {
            EXPECT_EQ(mixed_tuple_key6.type, MixedValueType::kTuple);
            auto *tuple_key6_ptr = (TupleMixedType *)(&mixed_tuple_key6);
            EXPECT_EQ(tuple_key6_ptr->count, 5);
        }

        // Key61: integer
        mixed_tuple_key6.InsertIntegerIntoTuple("key61", 200);
        {
            auto *value61_ptr = mixed_tuple_key6.GetFromTuple("key61");
            EXPECT_EQ(value61_ptr->type, MixedValueType::kInteger);
            auto *integer_value61_ptr = (IntegerMixedType *)(value61_ptr);
            EXPECT_EQ(integer_value61_ptr->value, 200);
        }

        // Key62: float
        mixed_tuple_key6.InsertFloatIntoTuple("key62", 6.1);
        {
            auto *value62_ptr = mixed_tuple_key6.GetFromTuple("key62");
            EXPECT_EQ(value62_ptr->type, MixedValueType::kFloat);
            auto *float_value62_ptr = (FloatMixedType *)(value62_ptr);
            EXPECT_FLOAT_EQ(float_value62_ptr->value, 6.1);
        }

        // Key63: short string
        mixed_tuple_key6.InsertStringIntoTuple("key63", "Hello World !!");
        {
            auto *value63_ptr = mixed_tuple_key6.GetFromTuple("key63");
            EXPECT_EQ(value63_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value63_ptr = (ShortStrMixedType *)(value63_ptr);
            String short_str(short_str_value63_ptr->ptr, short_str_value63_ptr->length);
            EXPECT_STREQ(short_str.c_str(), "Hello World !!");
        }

        // Key64: long string
        mixed_tuple_key6.InsertStringIntoTuple("key64", "Hello World Hello World");
        {
            auto *value64_ptr = mixed_tuple_key6.GetFromTuple("key64");
            EXPECT_EQ(value64_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value64_ptr = (LongStrMixedType *)(value64_ptr);

            String result(long_str_value64_ptr->ptr, long_str_value64_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World Hello World");
        }

        // Key65: null
        mixed_tuple_key6.InsertNullIntoTuple("key65");
        {
            auto *value65_ptr = mixed_tuple_key6.GetFromTuple("key65");
            EXPECT_EQ(value65_ptr->type, MixedValueType::kNull);
        }

        mixed_tuple1.MoveIntoTuple("key6", std::move(mixed_tuple_key6));
    }
    {
        auto *value6_ptr = mixed_tuple1.GetFromTuple("key6");
        EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
    }

    // Key7: array with 5 trivial type inside
    {
        MixedType mixed_array7 = MixedType::MakeArray(5);
        EXPECT_EQ(mixed_array7.type, MixedValueType::kArray);
        auto *array_ptr1 = (ArrayMixedType *)(&mixed_array7);
        EXPECT_EQ(array_ptr1->count, 5);

        // Integer
        mixed_array7.InsertIntegerIntoArray(4300, 0);
        {
            auto *value1_ptr = mixed_array7.GetByIndex(0);
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 4300);
        }

        // Float
        mixed_array7.InsertFloatIntoArray(5.5, 1);
        {
            auto *value2_ptr = mixed_array7.GetByIndex(1);
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 5.5);
        }

        // Short str
        mixed_array7.InsertStringIntoArray("FunnyHalloween", 2);
        {
            auto *value3_ptr = mixed_array7.GetByIndex(2);
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);
            EXPECT_STREQ(short_str_value_ptr->ptr, "FunnyHalloween");
        }

        // Long str
        mixed_array7.InsertStringIntoArray("FunnyHalloween OK", 3);
        {
            auto *value4_ptr = mixed_array7.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);
            String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "FunnyHalloween OK");
        }

        // Null
        mixed_array7.InsertNullIntoArray(4);
        {
            auto *value4_ptr = mixed_array7.GetByIndex(4);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
        }

        mixed_tuple1.MoveIntoTuple("key7", std::move(mixed_array7));
    }
    {
        auto *value7_ptr = mixed_tuple1.GetFromTuple("key7");
        EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
    }

    // Recheck all value and nested value in tuple;
    {
        EXPECT_EQ(mixed_tuple1.type, MixedValueType::kTuple);
        {
            auto *tuple_ptr1 = (TupleMixedType *)(&mixed_tuple1);
            EXPECT_EQ(tuple_ptr1->count, 7);
        }
        // Key1: integer
        {
            auto *value1_ptr = mixed_tuple1.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 100);
        }
        // Key2: float
        {
            auto *value2_ptr = mixed_tuple1.GetFromTuple("key2");
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.1);
        }
        // Key3: short string
        {
            auto *value3_ptr = mixed_tuple1.GetFromTuple("key3");
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World !!");
        }
        // Key4: long string
        {
            auto *value4_ptr = mixed_tuple1.GetFromTuple("key4");
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World Hello World");
        }
        // Key5: null
        {
            auto *value5_ptr = mixed_tuple1.GetFromTuple("key5");
            EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
        }
        // Key6: tuple with 5 trivial type inside
        {
            auto *value6_ptr = mixed_tuple1.GetFromTuple("key6");
            {
                EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                auto *tuple_key6_ptr = (TupleMixedType *)(value6_ptr);
                EXPECT_EQ(tuple_key6_ptr->count, 5);
            }
            // Key61: integer
            {
                auto *value61_ptr = value6_ptr->GetFromTuple("key61");
                EXPECT_EQ(value61_ptr->type, MixedValueType::kInteger);
                auto *integer_value61_ptr = (IntegerMixedType *)(value61_ptr);
                EXPECT_EQ(integer_value61_ptr->value, 200);
            }
            // Key62: float
            {
                auto *value62_ptr = value6_ptr->GetFromTuple("key62");
                EXPECT_EQ(value62_ptr->type, MixedValueType::kFloat);
                auto *float_value62_ptr = (FloatMixedType *)(value62_ptr);
                EXPECT_FLOAT_EQ(float_value62_ptr->value, 6.1);
            }
            // Key63: short string
            {
                auto *value63_ptr = value6_ptr->GetFromTuple("key63");
                EXPECT_EQ(value63_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value63_ptr = (ShortStrMixedType *)(value63_ptr);

                String result(short_str_value63_ptr->ptr, short_str_value63_ptr->length);
                EXPECT_STREQ(result.c_str(), "Hello World !!");
            }
            // Key64: long string
            {
                auto *value64_ptr = value6_ptr->GetFromTuple("key64");
                EXPECT_EQ(value64_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value64_ptr = (LongStrMixedType *)(value64_ptr);

                String result(long_str_value64_ptr->ptr, long_str_value64_ptr->length);
                EXPECT_STREQ(result.c_str(), "Hello World Hello World");
            }
            // Key65: null
            {
                auto *value65_ptr = value6_ptr->GetFromTuple("key65");
                EXPECT_EQ(value65_ptr->type, MixedValueType::kNull);
            }
        }
        // Key7: array with 5 trivial type inside
        {
            auto *value7_ptr = mixed_tuple1.GetFromTuple("key7");
            EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
            {
                auto *array_ptr1 = (ArrayMixedType *)(value7_ptr);
                EXPECT_EQ(array_ptr1->count, 5);
            }
            // Integer
            {
                auto *value1_ptr = value7_ptr->GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, 4300);
            }
            // Float
            {
                auto *value2_ptr = value7_ptr->GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, 5.5);
            }
            // Short str
            {
                auto *value3_ptr = value7_ptr->GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "FunnyHalloween");
            }
            // Long str
            {
                auto *value4_ptr = value7_ptr->GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "FunnyHalloween OK");
            }
            // Null
            {
                auto *value4_ptr = value7_ptr->GetByIndex(4);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
            }
        }
    }
}

TEST_F(MixedTypeTest, mixed_tuple3) {
    using namespace infinity;

    MixedType mixed_tuple3 = MixedType::MakeTuple(3);
    EXPECT_EQ(mixed_tuple3.type, MixedValueType::kTuple);
    auto *tuple_ptr3 = (TupleMixedType *)(&mixed_tuple3);
    EXPECT_EQ(tuple_ptr3->count, 3);

    // Key1: integer
    mixed_tuple3.InsertIntegerIntoTuple("key1key1", 300);
    {
        auto *value1_ptr = mixed_tuple3.GetFromTuple("key1key1");
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 300);
    }
    EXPECT_THROW(mixed_tuple3.InsertIntegerIntoTuple("key1key1", 400), ParserException);

    // Key2: short string
    mixed_tuple3.InsertStringIntoTuple("key1key1key1key1key1key2", "ABC");
    {
        auto *value1_ptr = mixed_tuple3.GetFromTuple("key1key1key1key1key1key2");
        EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
        auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "ABC");
    }
    EXPECT_THROW(mixed_tuple3.InsertIntegerIntoTuple("key1key1key1key1key1key2", 900), ParserException);

    // Key3: long string
    mixed_tuple3.InsertStringIntoTuple("key1key1key1key1key1key3", "ABCABCABCABCABCABCABCABCABC");
    {
        auto *value1_ptr = mixed_tuple3.GetFromTuple("key1key1key1key1key1key3");
        EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "ABCABCABCABCABCABCABCABCABC");
    }

    // Validate to throw the type exception, when insert element into a full tuple.
    EXPECT_THROW(mixed_tuple3.InsertStringIntoTuple("keyX", "ABC"), ParserException);

    // Validate to throw type exception, when create 0 capacity tuple.
    EXPECT_THROW(MixedType::MakeTuple(0), ParserException);
}

TEST_F(MixedTypeTest, mixed_array1) {
    using namespace infinity;

    MixedType mixed_array1 = MixedType::MakeArray(7);
    EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
    {
        auto *array_ptr1 = (ArrayMixedType *)(&mixed_array1);
        EXPECT_EQ(array_ptr1->count, 7);
    }

    // Exception
    EXPECT_THROW(mixed_array1.InsertIntegerIntoTuple("key1", 10), ParserException);

    // 1. Integer
    mixed_array1.InsertIntegerIntoArray(300, 0);
    {
        auto *value1_ptr = mixed_array1.GetByIndex(0);
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 300);
    }

    // 2. Float
    mixed_array1.InsertFloatIntoArray(1.5, 1);
    {
        auto *value2_ptr = mixed_array1.GetByIndex(1);
        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
    }

    // 3. Short str
    mixed_array1.InsertStringIntoArray("HappyHalloween", 2);
    {
        auto *value3_ptr = mixed_array1.GetByIndex(2);
        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "HappyHalloween");
    }

    // 4. Long str
    mixed_array1.InsertStringIntoArray("HappyHalloween OK", 3);
    {
        auto *value4_ptr = mixed_array1.GetByIndex(3);
        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "HappyHalloween OK");
    }

    // 5. Null
    mixed_array1.InsertNullIntoArray(4);
    {
        auto *value5_ptr = mixed_array1.GetByIndex(4);
        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
    }

    // 6. Tuple with 5 trivial type
    {
        MixedType mixed_tuple5 = MixedType::MakeTuple(5);
        EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
        {
            auto *tuple_ptr5 = (TupleMixedType *)(&mixed_tuple5);
            EXPECT_EQ(tuple_ptr5->count, 5);
        }
        // Key51: integer
        mixed_tuple5.InsertIntegerIntoTuple("key51", 1000);
        {
            auto *value1_ptr = mixed_tuple5.GetFromTuple("key51");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 1000);
        }

        // Key52: float
        mixed_tuple5.InsertFloatIntoTuple("key52", 1.001);
        {
            auto *value2_ptr = mixed_tuple5.GetFromTuple("key52");
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.001);
        }

        // Key53: short string
        mixed_tuple5.InsertStringIntoTuple("key53", "ABCDEFGHIJKLMN");
        {
            auto *value3_ptr = mixed_tuple5.GetFromTuple("key53");
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

            String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMN");
        }

        // Key54: long string
        mixed_tuple5.InsertStringIntoTuple("key54", "ABCDEFGHIJKLMNO");
        {
            auto *value4_ptr = mixed_tuple5.GetFromTuple("key54");
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

            String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMNO");
        }

        // Key55: null
        mixed_tuple5.InsertNullIntoTuple("key55");
        {
            auto *value5_ptr = mixed_tuple5.GetFromTuple("key55");
            EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
        }

        mixed_array1.CopyIntoArray(mixed_tuple5, 5);
        {
            //
            // Check the value of mixed tuple5 after copy
            //
            EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
            {
                auto *tuple_ptr5 = (TupleMixedType *)(&mixed_tuple5);
                EXPECT_EQ(tuple_ptr5->count, 5);
            }

            // Key51: integer
            {
                auto *value1_ptr = mixed_tuple5.GetFromTuple("key51");
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, 1000);
            }

            // Key52: float
            {
                auto *value2_ptr = mixed_tuple5.GetFromTuple("key52");
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, 1.001);
            }

            // Key53: short string
            {
                auto *value3_ptr = mixed_tuple5.GetFromTuple("key53");
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMN");
            }

            // Key54: long string
            {
                auto *value4_ptr = mixed_tuple5.GetFromTuple("key54");
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMNO");
            }

            // Key55: null
            {
                auto *value5_ptr = mixed_tuple5.GetFromTuple("key55");
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }
        }
    }
    {
        auto *value5_ptr = mixed_array1.GetByIndex(5);
        EXPECT_EQ(value5_ptr->type, MixedValueType::kTuple);
    }

    // 7. Array with 5 trivial type
    {
        MixedType nested_array = MixedType::MakeArray(5);
        EXPECT_EQ(nested_array.type, MixedValueType::kArray);
        {
            auto *nested_array_ptr1 = (ArrayMixedType *)(&nested_array);
            EXPECT_EQ(nested_array_ptr1->count, 5);
        }

        // Integer
        nested_array.InsertIntegerIntoArray(3000, 0);
        {
            auto *value1_ptr = nested_array.GetByIndex(0);
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 3000);
        }

        // Float
        nested_array.InsertFloatIntoArray(1.5, 1);
        {
            auto *value2_ptr = nested_array.GetByIndex(1);
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
        }

        // Short str
        nested_array.InsertStringIntoArray("MNOPQRSTUVWXYZ", 2);
        {
            auto *value3_ptr = nested_array.GetByIndex(2);
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

            String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "MNOPQRSTUVWXYZ");
        }

        // Long str
        nested_array.InsertStringIntoArray("LMNOPQRSTUVWXYZ", 3);
        {
            auto *value4_ptr = nested_array.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

            String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "LMNOPQRSTUVWXYZ");
        }

        // Null
        nested_array.InsertNullIntoArray(4);
        {
            auto *value4_ptr = nested_array.GetByIndex(4);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
        }

        mixed_array1.CopyIntoArray(nested_array, 6);

        {
            //
            // Check the value of nested_array after copy
            //
            EXPECT_EQ(nested_array.type, MixedValueType::kArray);
            {
                auto *nested_array_ptr1 = (ArrayMixedType *)(&nested_array);
                EXPECT_EQ(nested_array_ptr1->count, 5);
            }

            // Integer
            {
                auto *value1_ptr = nested_array.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, 3000);
            }

            // Float
            {
                auto *value2_ptr = nested_array.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
            }

            // Short str
            {
                auto *value3_ptr = nested_array.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "MNOPQRSTUVWXYZ");
            }

            // Long str
            {
                auto *value4_ptr = nested_array.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), "LMNOPQRSTUVWXYZ");
            }

            // Null
            {
                auto *value4_ptr = nested_array.GetByIndex(4);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
            }
        }
    }
    {
        auto *value6_ptr = mixed_array1.GetByIndex(6);
        EXPECT_EQ(value6_ptr->type, MixedValueType::kArray);
    }

    // Recheck all value and nested value in array;
    {
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        {
            auto *array_ptr1 = (ArrayMixedType *)(&mixed_array1);
            EXPECT_EQ(array_ptr1->count, 7);
        }
        // 1. Integer
        {
            auto *value1_ptr = mixed_array1.GetByIndex(0);
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 300);
        }
        // 2. Float
        {
            auto *value2_ptr = mixed_array1.GetByIndex(1);
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
        }
        // 3. Short str
        {
            auto *value3_ptr = mixed_array1.GetByIndex(2);
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

            String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "HappyHalloween");
        }
        // 4. Long str
        {
            auto *value4_ptr = mixed_array1.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

            String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "HappyHalloween OK");
        }
        // 5. Null
        {
            auto *value5_ptr = mixed_array1.GetByIndex(4);
            EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
        }
        // 6. Tuple
        {
            auto *value6_ptr = mixed_array1.GetByIndex(5);
            {
                EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                {
                    auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                    EXPECT_EQ(tuple_ptr5->count, 5);
                }
                // Key51: integer
                {
                    auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                    EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                    auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                    EXPECT_EQ(integer_value_ptr->value, 1000);
                }
                // Key52: float
                {
                    auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                    EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                    auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                    EXPECT_FLOAT_EQ(float_value_ptr->value, 1.001);
                }
                // Key53: short string
                {
                    auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                    EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                    auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                    String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMN");
                }
                // Key54: long string
                {
                    auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                    EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                    auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                    String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMNO");
                }

                // Key55: null
                {
                    auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                    EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                }
            }
        }
        // 7. Array
        {
            auto *value7_ptr = mixed_array1.GetByIndex(6);
            {
                EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                {
                    auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                    EXPECT_EQ(nested_array_ptr1->count, 5);
                }
                // Integer
                {
                    auto *value1_ptr = value7_ptr->GetByIndex(0);
                    EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                    auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                    EXPECT_EQ(integer_value_ptr->value, 3000);
                }
                // Float
                {
                    auto *value2_ptr = value7_ptr->GetByIndex(1);
                    EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                    auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                    EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
                }
                // Short str
                {
                    auto *value3_ptr = value7_ptr->GetByIndex(2);
                    EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                    auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                    String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "MNOPQRSTUVWXYZ");
                }
                // Long str
                {
                    auto *value4_ptr = value7_ptr->GetByIndex(3);
                    EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                    auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                    String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "LMNOPQRSTUVWXYZ");
                }

                // Null
                {
                    auto *value4_ptr = value7_ptr->GetByIndex(4);
                    EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                }
            }
        }
    }
}

TEST_F(MixedTypeTest, mixed_array2) {
    using namespace infinity;

    MixedType mixed_array1 = MixedType::MakeArray(7);
    EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
    auto *array_ptr1 = (ArrayMixedType *)(&mixed_array1);
    EXPECT_EQ(array_ptr1->count, 7);

    // Exception
    EXPECT_THROW(mixed_array1.InsertIntegerIntoTuple("key1", 10), ParserException);

    // Integer
    mixed_array1.InsertIntegerIntoArray(300, 0);
    {
        auto *value1_ptr = mixed_array1.GetByIndex(0);
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 300);
    }

    // Float
    mixed_array1.InsertFloatIntoArray(1.5, 1);
    {
        auto *value2_ptr = mixed_array1.GetByIndex(1);
        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
    }

    // Short str
    mixed_array1.InsertStringIntoArray("HappyHalloween", 2);
    {
        auto *value3_ptr = mixed_array1.GetByIndex(2);
        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);
        String short_str(short_str_value_ptr->ptr, short_str_value_ptr->length);
        EXPECT_STREQ(short_str.c_str(), "HappyHalloween");
    }

    // Long str
    mixed_array1.InsertStringIntoArray("HappyHalloween OK", 3);
    {
        auto *value4_ptr = mixed_array1.GetByIndex(3);
        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);
        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "HappyHalloween OK");
    }

    // Null
    mixed_array1.InsertNullIntoArray(4);
    {
        auto *value4_ptr = mixed_array1.GetByIndex(4);
        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
    }

    // Tuple with 5 trivial type
    {
        MixedType mixed_tuple5 = MixedType::MakeTuple(5);
        EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
        auto *tuple_ptr5 = (TupleMixedType *)(&mixed_tuple5);
        EXPECT_EQ(tuple_ptr5->count, 5);

        // Key51: integer
        mixed_tuple5.InsertIntegerIntoTuple("key51", 1000);
        {
            auto *value1_ptr = mixed_tuple5.GetFromTuple("key51");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 1000);
        }

        // Key52: float
        mixed_tuple5.InsertFloatIntoTuple("key52", 1.001);
        {
            auto *value2_ptr = mixed_tuple5.GetFromTuple("key52");
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.001);
        }

        // Key53: short string
        mixed_tuple5.InsertStringIntoTuple("key53", "ABCDEFGHIJKLMN");
        {
            auto *value3_ptr = mixed_tuple5.GetFromTuple("key53");
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);
            String short_str(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(short_str.c_str(), "ABCDEFGHIJKLMN");
        }

        // Key54: long string
        mixed_tuple5.InsertStringIntoTuple("key54", "ABCDEFGHIJKLMNO");
        {
            auto *value4_ptr = mixed_tuple5.GetFromTuple("key54");
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);
            String long_str(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(long_str.c_str(), "ABCDEFGHIJKLMNO");
        }

        // Key55: null
        mixed_tuple5.InsertNullIntoTuple("key55");
        {
            auto *value5_ptr = mixed_tuple5.GetFromTuple("key55");
            EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
        }

        mixed_array1.MoveIntoArray(std::move(mixed_tuple5), 5);

        // Nested_array should be reset to empty
        EXPECT_EQ(mixed_tuple5.type, MixedValueType::kInvalid);
    }
    {
        auto *value5_ptr = mixed_array1.GetByIndex(5);
        EXPECT_EQ(value5_ptr->type, MixedValueType::kTuple);
    }

    // Array with 5 trivial type
    {
        MixedType nested_array = MixedType::MakeArray(5);
        EXPECT_EQ(nested_array.type, MixedValueType::kArray);
        auto *nested_array_ptr1 = (ArrayMixedType *)(&nested_array);
        EXPECT_EQ(nested_array_ptr1->count, 5);

        // Integer
        nested_array.InsertIntegerIntoArray(3000, 0);
        {
            auto *value1_ptr = nested_array.GetByIndex(0);
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 3000);
        }

        // Float
        nested_array.InsertFloatIntoArray(1.5, 1);
        {
            auto *value2_ptr = nested_array.GetByIndex(1);
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
        }

        // Short str
        nested_array.InsertStringIntoArray("MNOPQRSTUVWXYZ", 2);
        {
            auto *value3_ptr = nested_array.GetByIndex(2);
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);
            String short_str(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(short_str.c_str(), "MNOPQRSTUVWXYZ");
        }

        // Long str
        nested_array.InsertStringIntoArray("LMNOPQRSTUVWXYZ", 3);
        {
            auto *value4_ptr = nested_array.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);
            String long_str(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(long_str.c_str(), "LMNOPQRSTUVWXYZ");
        }

        // Null
        nested_array.InsertNullIntoArray(4);
        {
            auto *value4_ptr = nested_array.GetByIndex(4);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
        }

        mixed_array1.MoveIntoArray(std::move(nested_array), 6);

        // Nested_array should be reset to empty
        EXPECT_EQ(nested_array.type, MixedValueType::kInvalid);
    }
    {
        auto *value6_ptr = mixed_array1.GetByIndex(6);
        EXPECT_EQ(value6_ptr->type, MixedValueType::kArray);
    }

    // Recheck all value and nested value in array;
    {
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        {
            auto *array_ptr1 = (ArrayMixedType *)(&mixed_array1);
            EXPECT_EQ(array_ptr1->count, 7);
        }
        // 1. Integer
        {
            auto *value1_ptr = mixed_array1.GetByIndex(0);
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 300);
        }
        // 2. Float
        {
            auto *value2_ptr = mixed_array1.GetByIndex(1);
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
        }
        // 3. Short str
        {
            auto *value3_ptr = mixed_array1.GetByIndex(2);
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

            String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "HappyHalloween");
        }
        // 4. Long str
        {
            auto *value4_ptr = mixed_array1.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

            String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), "HappyHalloween OK");
        }
        // 5. Null
        {
            auto *value5_ptr = mixed_array1.GetByIndex(4);
            EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
        }
        // 6. Tuple
        {
            auto *value6_ptr = mixed_array1.GetByIndex(5);
            {
                EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                {
                    auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                    EXPECT_EQ(tuple_ptr5->count, 5);
                }
                // Key51: integer
                {
                    auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                    EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                    auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                    EXPECT_EQ(integer_value_ptr->value, 1000);
                }
                // Key52: float
                {
                    auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                    EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                    auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                    EXPECT_FLOAT_EQ(float_value_ptr->value, 1.001);
                }
                // Key53: short string
                {
                    auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                    EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                    auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                    String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMN");
                }
                // Key54: long string
                {
                    auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                    EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                    auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                    String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "ABCDEFGHIJKLMNO");
                }

                // Key55: null
                {
                    auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                    EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                }
            }
        }
        // 7. Array
        {
            auto *value7_ptr = mixed_array1.GetByIndex(6);
            {
                EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                {
                    auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                    EXPECT_EQ(nested_array_ptr1->count, 5);
                }
                // Integer
                {
                    auto *value1_ptr = value7_ptr->GetByIndex(0);
                    EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                    auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                    EXPECT_EQ(integer_value_ptr->value, 3000);
                }
                // Float
                {
                    auto *value2_ptr = value7_ptr->GetByIndex(1);
                    EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                    auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                    EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
                }
                // Short str
                {
                    auto *value3_ptr = value7_ptr->GetByIndex(2);
                    EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                    auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                    String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "MNOPQRSTUVWXYZ");
                }
                // Long str
                {
                    auto *value4_ptr = value7_ptr->GetByIndex(3);
                    EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                    auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                    String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                    EXPECT_STREQ(result.c_str(), "LMNOPQRSTUVWXYZ");
                }

                // Null
                {
                    auto *value4_ptr = value7_ptr->GetByIndex(4);
                    EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                }
            }
        }
    }
}

TEST_F(MixedTypeTest, mixed_array3) {
    using namespace infinity;

    MixedType mixed_array1 = MixedType::MakeArray(2);
    EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
    auto *array_ptr1 = (ArrayMixedType *)(&mixed_array1);
    EXPECT_EQ(array_ptr1->count, 2);

    // Integer
    mixed_array1.InsertIntegerIntoArray(300, 0);
    {
        auto *value1_ptr = mixed_array1.GetByIndex(0);
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 300);
    }

    // Overwrite it by another mixed value
    mixed_array1.InsertStringIntoArray("XXXXXXXXXXXXXXXXXXXXXXXX", 0);
    {
        auto *value1_ptr = mixed_array1.GetByIndex(0);
        EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
        EXPECT_STREQ(result.c_str(), "XXXXXXXXXXXXXXXXXXXXXXXX");
    }

    mixed_array1.InsertFloatIntoArray(4.3, 0);
    {
        auto *value1_ptr = mixed_array1.GetByIndex(0);
        EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 4.3);
    }

    // Float
    mixed_array1.InsertFloatIntoArray(1.5, 1);

    {
        auto *value1_ptr = mixed_array1.GetByIndex(1);
        EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
    }

    // Set value into invalid index
    EXPECT_THROW(mixed_array1.InsertFloatIntoArray(1.5, 2), ParserException);
    EXPECT_THROW(MixedType::MakeArray(0), ParserException);
}
