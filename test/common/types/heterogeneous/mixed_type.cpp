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

    String another = "Aello the world";
    EXPECT_FALSE(long_string_mixed_ptr->Equal(another));
    another = "Hellow";
    EXPECT_FALSE(long_string_mixed_ptr->Equal(another));
    another = "Hello the world";
    EXPECT_TRUE(long_string_mixed_ptr->Equal(another));

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

TEST_F(MixedTypeTest, mixed_tuple) {
    using namespace infinity;

    MixedType mixed_tuple1 = MixedType::MakeTuple(7);
    EXPECT_EQ(mixed_tuple1.type, MixedValueType::kTuple);
    auto *tuple_ptr1 = (TupleMixedType *) (&mixed_tuple1);
    EXPECT_EQ(tuple_ptr1->count, 7);

    // Exception
    EXPECT_THROW(mixed_tuple1.InsertIntegerIntoArray(100, 10), TypeException);

    // Key1: integer
    mixed_tuple1.InsertIntegerIntoTuple("key1", 100);
    {
        auto *value1_ptr = mixed_tuple1.GetFromTuple("key1");
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *) (value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 100);
    }

    // Key2: float
    mixed_tuple1.InsertFloatIntoTuple("key2", 1.1);
    {
        auto *value2_ptr = mixed_tuple1.GetFromTuple("key2");
        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *) (value2_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 1.1);
    }

    // Key3: short string
    mixed_tuple1.InsertStringIntoTuple("key3", "Hello World !!");
    {
        auto *value3_ptr = mixed_tuple1.GetFromTuple("key3");
        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
        auto *short_str_value_ptr = (ShortStrMixedType *) (value3_ptr);
        EXPECT_STREQ(short_str_value_ptr->ptr, "Hello World !!");
    }

    // Key4: long string
    mixed_tuple1.InsertStringIntoTuple("key4", "Hello World Hello World");
    {
        auto *value4_ptr = mixed_tuple1.GetFromTuple("key4");
        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *) (value4_ptr);
        EXPECT_STREQ(long_str_value_ptr->ptr, "Hello World Hello World");
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
            auto *tuple_key6_ptr = (TupleMixedType *) (&mixed_tuple_key6);
            EXPECT_EQ(tuple_key6_ptr->count, 5);
        }

        // Key61: integer
        mixed_tuple_key6.InsertIntegerIntoTuple("key61", 200);
        {
            auto *value61_ptr = mixed_tuple_key6.GetFromTuple("key61");
            EXPECT_EQ(value61_ptr->type, MixedValueType::kInteger);
            auto *integer_value61_ptr = (IntegerMixedType *) (value61_ptr);
            EXPECT_EQ(integer_value61_ptr->value, 200);
        }

        // Key62: float
        mixed_tuple_key6.InsertFloatIntoTuple("key62", 6.1);
        {
            auto *value62_ptr = mixed_tuple_key6.GetFromTuple("key62");
            EXPECT_EQ(value62_ptr->type, MixedValueType::kFloat);
            auto *float_value62_ptr = (FloatMixedType *) (value62_ptr);
            EXPECT_FLOAT_EQ(float_value62_ptr->value, 6.1);
        }

        // Key63: short string
        mixed_tuple_key6.InsertStringIntoTuple("key63", "Hello World !!");
        {
            auto *value63_ptr = mixed_tuple_key6.GetFromTuple("key63");
            EXPECT_EQ(value63_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value63_ptr = (ShortStrMixedType *) (value63_ptr);
            EXPECT_STREQ(short_str_value63_ptr->ptr, "Hello World !!");
        }

        // Key64: long string
        mixed_tuple_key6.InsertStringIntoTuple("key64", "Hello World Hello World");
        {
            auto *value64_ptr = mixed_tuple_key6.GetFromTuple("key64");
            EXPECT_EQ(value64_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value64_ptr = (LongStrMixedType *) (value64_ptr);
            
            String result(long_str_value64_ptr->ptr, long_str_value64_ptr->length);
            EXPECT_STREQ(result.c_str(), "Hello World Hello World");
        }

        // Key65: null
        mixed_tuple_key6.InsertNullIntoTuple("key65");
        {
            auto *value65_ptr = mixed_tuple_key6.GetFromTuple("key65");
            EXPECT_EQ(value65_ptr->type, MixedValueType::kNull);
        }

//        mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);
    }
//    {
//        auto *value6_ptr = mixed_tuple1.GetFromTuple("key6");
//        EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
//    }

    // Key7: array with 5 trivial type inside

}

TEST_F(MixedTypeTest, mixed_array) {
    using namespace infinity;

    MixedType mixed_array1 = MixedType::MakeArray(7);
    EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
    auto *array_ptr1 = (ArrayMixedType *) (&mixed_array1);
    EXPECT_EQ(array_ptr1->count, 7);

    // Exception
    EXPECT_THROW(mixed_array1.InsertIntegerIntoTuple("key1", 10), TypeException);

    // Integer
    mixed_array1.InsertIntegerIntoArray(300, 0);
    {
        auto *value1_ptr = mixed_array1.GetByIndex(0);
        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
        auto *integer_value_ptr = (IntegerMixedType *) (value1_ptr);
        EXPECT_EQ(integer_value_ptr->value, 300);
    }

    // Float
    mixed_array1.InsertFloatIntoArray(1.5, 1);
    {
        auto *value2_ptr = mixed_array1.GetByIndex(1);
        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
        auto *float_value_ptr = (FloatMixedType *) (value2_ptr);
        EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
    }

    // Short str
    mixed_array1.InsertStringIntoArray("HappyHalloween", 2);
    {
        auto *value3_ptr = mixed_array1.GetByIndex(2);
        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
        auto *short_str_value_ptr = (ShortStrMixedType *) (value3_ptr);
        EXPECT_STREQ(short_str_value_ptr->ptr, "HappyHalloween");
    }

    // Long str
    mixed_array1.InsertStringIntoArray("HappyHalloween OK", 3);
    {
        auto *value4_ptr = mixed_array1.GetByIndex(3);
        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
        auto *long_str_value_ptr = (LongStrMixedType *) (value4_ptr);
        EXPECT_STREQ(long_str_value_ptr->ptr, "HappyHalloween OK");
    }

    // Null
    mixed_array1.InsertNullIntoArray(3);
    {
        auto *value4_ptr = mixed_array1.GetByIndex(3);
        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
    }

    // Tuple with 5 trivial type
    {
        MixedType mixed_tuple5 = MixedType::MakeTuple(5);
        EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
        auto *tuple_ptr5 = (TupleMixedType *) (&mixed_tuple5);
        EXPECT_EQ(tuple_ptr5->count, 5);

        // Key51: integer
        mixed_tuple5.InsertIntegerIntoTuple("key51", 1000);
        {
            auto *value1_ptr = mixed_tuple5.GetFromTuple("key51");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *) (value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 1000);
        }

        // Key52: float
        mixed_tuple5.InsertFloatIntoTuple("key52", 1.001);
        {
            auto *value2_ptr = mixed_tuple5.GetFromTuple("key52");
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *) (value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.001);
        }

        // Key53: short string
        mixed_tuple5.InsertStringIntoTuple("key53", "ABCDEFGHIJKLMN");
        {
            auto *value3_ptr = mixed_tuple5.GetFromTuple("key53");
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *) (value3_ptr);
            EXPECT_STREQ(short_str_value_ptr->ptr, "ABCDEFGHIJKLMN");
        }

        // Key54: long string
        mixed_tuple5.InsertStringIntoTuple("key54", "ABCDEFGHIJKLMNO");
        {
            auto *value4_ptr = mixed_tuple5.GetFromTuple("key54");
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *) (value4_ptr);
            EXPECT_STREQ(long_str_value_ptr->ptr, "ABCDEFGHIJKLMNO");
        }

        // Key55: null
        mixed_tuple5.InsertNullIntoTuple("key55");
        {
            auto *value5_ptr = mixed_tuple5.GetFromTuple("key55");
            EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
        }

//        mixed_array1.MoveIntoArray(std::move(mixed_tuple5), 4);
    }
//    {
//        auto *value5_ptr = mixed_array1.GetByIndex(4);
//        EXPECT_EQ(value5_ptr->type, MixedValueType::kTuple);
//    }

    // Array with 5 trivial type
    {
        MixedType nested_array = MixedType::MakeArray(5);
        EXPECT_EQ(nested_array.type, MixedValueType::kArray);
        auto *nested_array_ptr1 = (ArrayMixedType *) (&nested_array);
        EXPECT_EQ(nested_array_ptr1->count, 5);

        // Integer
        nested_array.InsertIntegerIntoArray(3000, 0);
        {
            auto *value1_ptr = nested_array.GetByIndex(0);
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *) (value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 3000);
        }

        // Float
        nested_array.InsertFloatIntoArray(1.5, 1);
        {
            auto *value2_ptr = nested_array.GetByIndex(1);
            EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *) (value2_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, 1.5);
        }

        // Short str
        nested_array.InsertStringIntoArray("MNOPQRSTUVWXYZ", 2);
        {
            auto *value3_ptr = nested_array.GetByIndex(2);
            EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *) (value3_ptr);
            EXPECT_STREQ(short_str_value_ptr->ptr, "MNOPQRSTUVWXYZ");
        }

        // Long str
        nested_array.InsertStringIntoArray("LMNOPQRSTUVWXYZ", 3);
        {
            auto *value4_ptr = nested_array.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *) (value4_ptr);
            EXPECT_STREQ(long_str_value_ptr->ptr, "LMNOPQRSTUVWXYZ");
        }

        // Null
        nested_array.InsertNullIntoArray(3);
        {
            auto *value4_ptr = nested_array.GetByIndex(3);
            EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
        }

//        mixed_array1.CopyIntoArray(nested_array, 5);
    }
//    {
//        auto *value6_ptr = mixed_array1.GetByIndex(5);
//        EXPECT_EQ(value6_ptr->type, MixedValueType::kArray);
//    }

    // TODO: need to recheck all nested value.
}
