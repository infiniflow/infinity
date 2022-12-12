//
// Created by JinHai on 2022/12/11.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"

class ColumnVectorMixedTest : public BaseTest {
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

TEST_F(ColumnVectorMixedTest, mixed_integer_a) {
    using namespace infinity;

    DataType data_type(LogicalType::kMixed);
    ColumnVector col_mixed(data_type, ColumnVectorType::kFlat);
    col_mixed.Initialize();

    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        Value v = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_integer1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto* integer_mixed_ptr = (IntegerMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto* integer_mixed_ptr = (IntegerMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
    }

    EXPECT_EQ(col_mixed.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        Value v = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_integer1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto* integer_mixed_ptr = (IntegerMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reset();
    EXPECT_EQ(col_mixed.capacity(), 0);
    EXPECT_EQ(col_mixed.tail_index_, 0);
//    EXPECT_EQ(col_mixed.data_type_size_, 0);
    EXPECT_EQ(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.initialized, false);

    // ====
    col_mixed.Initialize();
    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        Value v = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_integer1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto* integer_mixed_ptr = (IntegerMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_float_a) {
    using namespace infinity;

    DataType data_type(LogicalType::kMixed);
    ColumnVector col_mixed(data_type, ColumnVectorType::kFlat);
    col_mixed.Initialize();

    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        Value v = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_float1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto* float_mixed_ptr = (FloatMixedType*)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto* float_mixed_ptr = (FloatMixedType*)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
    }

    EXPECT_EQ(col_mixed.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        Value v = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_float1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto* float_mixed_ptr = (FloatMixedType*)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reset();
    EXPECT_EQ(col_mixed.capacity(), 0);
    EXPECT_EQ(col_mixed.tail_index_, 0);
//    EXPECT_EQ(col_mixed.data_type_size_, 0);
    EXPECT_EQ(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.initialized, false);

    // ====
    col_mixed.Initialize();
    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        Value v = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_float1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto* float_mixed_ptr = (FloatMixedType*)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_short_str_a) {
    using namespace infinity;

    DataType data_type(LogicalType::kMixed);
    ColumnVector col_mixed(data_type, ColumnVectorType::kFlat);
    col_mixed.Initialize();

    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto* short_str_mixed_ptr = (ShortStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str = std::to_string(i);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto* short_str_mixed_ptr = (ShortStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
    }

    EXPECT_EQ(col_mixed.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto* short_str_mixed_ptr = (ShortStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reset();
    EXPECT_EQ(col_mixed.capacity(), 0);
    EXPECT_EQ(col_mixed.tail_index_, 0);
//    EXPECT_EQ(col_mixed.data_type_size_, 0);
    EXPECT_EQ(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.initialized, false);

    // ====
    col_mixed.Initialize();
    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto* short_str_mixed_ptr = (ShortStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_long_str_a) {
    using namespace infinity;

    DataType data_type(LogicalType::kMixed);
    ColumnVector col_mixed(data_type, ColumnVectorType::kFlat);
    col_mixed.Initialize();

    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reserve(DEFAULT_VECTOR_SIZE* 2);


    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
    }

    EXPECT_EQ(col_mixed.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reset();

    EXPECT_EQ(col_mixed.capacity(), 0);
    EXPECT_EQ(col_mixed.tail_index_, 0);
//    EXPECT_EQ(col_mixed.data_type_size_, 0);
    EXPECT_EQ(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.initialized, false);

    // ====
    col_mixed.Initialize();
    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_null_a) {
    using namespace infinity;

    DataType data_type(LogicalType::kMixed);
    ColumnVector col_mixed(data_type, ColumnVectorType::kFlat);
    col_mixed.Initialize();

    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_null = MixedType::MakeNull();
        Value v = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_null);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reserve(DEFAULT_VECTOR_SIZE* 2);


    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
    }

    EXPECT_EQ(col_mixed.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_null = MixedType::MakeNull();
        Value v = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_null);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reset();

    EXPECT_EQ(col_mixed.capacity(), 0);
    EXPECT_EQ(col_mixed.tail_index_, 0);
//    EXPECT_EQ(col_mixed.data_type_size_, 0);
    EXPECT_EQ(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.initialized, false);

    // ====
    col_mixed.Initialize();
    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_null = MixedType::MakeNull();
        Value v = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_null);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_missing_a) {
    using namespace infinity;

    DataType data_type(LogicalType::kMixed);
    ColumnVector col_mixed(data_type, ColumnVectorType::kFlat);
    col_mixed.Initialize();

    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        Value v = Value::MakeMixedData(mixed_missing);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_missing);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reserve(DEFAULT_VECTOR_SIZE* 2);


    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
    }

    EXPECT_EQ(col_mixed.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        Value v = Value::MakeMixedData(mixed_missing);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_missing);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reset();

    EXPECT_EQ(col_mixed.capacity(), 0);
    EXPECT_EQ(col_mixed.tail_index_, 0);
//    EXPECT_EQ(col_mixed.data_type_size_, 0);
    EXPECT_EQ(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.initialized, false);

    // ====
    col_mixed.Initialize();
    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        Value v = Value::MakeMixedData(mixed_missing);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_missing);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_tuple_a) {
    using namespace infinity;

    DataType data_type(LogicalType::kMixed);
    ColumnVector col_mixed(data_type, ColumnVectorType::kFlat);
    col_mixed.Initialize();

    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
#if 0
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reserve(DEFAULT_VECTOR_SIZE* 2);


    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
    }

    EXPECT_EQ(col_mixed.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }

    col_mixed.Reset();

    EXPECT_EQ(col_mixed.capacity(), 0);
    EXPECT_EQ(col_mixed.tail_index_, 0);
//    EXPECT_EQ(col_mixed.data_type_size_, 0);
    EXPECT_EQ(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.initialized, false);

    // ====
    col_mixed.Initialize();
    EXPECT_THROW(col_mixed.SetDataType(DataType(LogicalType::kMixed)), std::logic_error);
    EXPECT_THROW(col_mixed.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_mixed.Size(), 0);
    EXPECT_THROW(col_mixed.ToString(), std::logic_error);
    EXPECT_THROW(col_mixed.GetValue(0), std::logic_error);
    EXPECT_EQ(col_mixed.tail_index_, 0);
    EXPECT_EQ(col_mixed.data_type_size_, 16);
    EXPECT_NE(col_mixed.data_ptr_, nullptr);
    EXPECT_EQ(col_mixed.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_mixed.data_type(), data_type);
    EXPECT_EQ(col_mixed.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_mixed.buffer_, nullptr);
    EXPECT_EQ(col_mixed.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_mixed.initialized);
    col_mixed.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_mixed.data_ptr_;
    EXPECT_EQ(col_mixed.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_mixed.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        col_mixed.AppendValue(v);
        Value vx = col_mixed.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto* long_str_mixed_ptr = (LongStrMixedType*)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(col_mixed.GetValue(i + 1), std::logic_error);
    }
#endif
}
