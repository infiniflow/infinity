//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/decimal16_info.h"
#include "common/types/info/decimal32_info.h"
#include "common/types/info/decimal64_info.h"
#include "common/types/info/decimal128_info.h"

class ColumnVectorDecimalTest : public BaseTest {
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

TEST_F(ColumnVectorDecimalTest, flat_decimal16) {
    using namespace infinity;

    auto decimal16_info = Decimal16Info::Make(4, 4);
    DataType data_type(LogicalType::kDecimal16, decimal16_info);
    ColumnVector col_decimal16(data_type, ColumnVectorType::kFlat);
    col_decimal16.Initialize();

    EXPECT_THROW(col_decimal16.SetDataType(DataType(LogicalType::kDecimal16)), TypeException);
    EXPECT_THROW(col_decimal16.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal16.Size(), 0);
    EXPECT_THROW(col_decimal16.ToString(), TypeException);
    EXPECT_THROW(col_decimal16.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal16.tail_index_, 0);
    EXPECT_EQ(col_decimal16.data_type_size_, 2);
    EXPECT_NE(col_decimal16.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal16.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal16.data_type(), data_type);
    EXPECT_EQ(col_decimal16.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal16.buffer_, nullptr);
    EXPECT_NE(col_decimal16.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal16.initialized);
    col_decimal16.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_decimal16.data_ptr_;
    EXPECT_EQ(col_decimal16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal16.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal16T decimal16(static_cast<i16>(i));
        Value v = Value::MakeDecimal16(decimal16, decimal16_info);
        col_decimal16.AppendValue(v);
        Value vx = col_decimal16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal16);
        EXPECT_EQ(vx.value_.decimal16.value, static_cast<i16>(i));
        EXPECT_THROW(col_decimal16.GetValue(i + 1), TypeException);
    }

    col_decimal16.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_decimal16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal16);
        EXPECT_EQ(vx.value_.decimal16.value, static_cast<i16>(i));
    }

    EXPECT_EQ(col_decimal16.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal16.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal16T decimal16(static_cast<i16>(i));
        Value v = Value::MakeDecimal16(decimal16, decimal16_info);
        col_decimal16.AppendValue(v);
        Value vx = col_decimal16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal16);
        EXPECT_EQ(vx.value_.decimal16.value, static_cast<i16>(i));
        EXPECT_THROW(col_decimal16.GetValue(i + 1), TypeException);
    }

    col_decimal16.Reset();
    EXPECT_EQ(col_decimal16.capacity(), 0);
    EXPECT_EQ(col_decimal16.tail_index_, 0);
    EXPECT_NE(col_decimal16.buffer_, nullptr);
    EXPECT_NE(col_decimal16.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal16.initialized, false);

    // ====
    col_decimal16.Initialize();
    EXPECT_THROW(col_decimal16.SetDataType(DataType(LogicalType::kDecimal16)), TypeException);
    EXPECT_THROW(col_decimal16.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal16.Size(), 0);
    EXPECT_THROW(col_decimal16.ToString(), TypeException);
    EXPECT_THROW(col_decimal16.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal16.tail_index_, 0);
    EXPECT_EQ(col_decimal16.data_type_size_, 2);
    EXPECT_NE(col_decimal16.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal16.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal16.data_type(), data_type);
    EXPECT_EQ(col_decimal16.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal16.buffer_, nullptr);
    EXPECT_NE(col_decimal16.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal16.initialized);
    col_decimal16.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_decimal16.data_ptr_;
    EXPECT_EQ(col_decimal16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal16.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal16T decimal16(static_cast<i16>(i));
        Value v = Value::MakeDecimal16(decimal16, decimal16_info);
        col_decimal16.AppendValue(v);
        Value vx = col_decimal16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal16);
        EXPECT_EQ(vx.value_.decimal16.value, static_cast<i16>(i));
        EXPECT_THROW(col_decimal16.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorDecimalTest, flat_decimal32) {
    using namespace infinity;

    auto decimal32_info = Decimal32Info::Make(9, 9);
    DataType data_type(LogicalType::kDecimal32, decimal32_info);
    ColumnVector col_decimal32(data_type, ColumnVectorType::kFlat);
    col_decimal32.Initialize();

    EXPECT_THROW(col_decimal32.SetDataType(DataType(LogicalType::kDecimal32)), TypeException);
    EXPECT_THROW(col_decimal32.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal32.Size(), 0);
    EXPECT_THROW(col_decimal32.ToString(), TypeException);
    EXPECT_THROW(col_decimal32.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal32.tail_index_, 0);
    EXPECT_EQ(col_decimal32.data_type_size_, 4);
    EXPECT_NE(col_decimal32.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal32.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal32.data_type(), data_type);
    EXPECT_EQ(col_decimal32.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal32.buffer_, nullptr);
    EXPECT_NE(col_decimal32.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal32.initialized);
    col_decimal32.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_decimal32.data_ptr_;
    EXPECT_EQ(col_decimal32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal32.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal32T decimal32(static_cast<i32>(i));
        Value v = Value::MakeDecimal32(decimal32, decimal32_info);
        col_decimal32.AppendValue(v);
        Value vx = col_decimal32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal32);
        EXPECT_EQ(vx.value_.decimal32.value, static_cast<i32>(i));
        EXPECT_THROW(col_decimal32.GetValue(i + 1), TypeException);
    }

    col_decimal32.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_decimal32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal32);
        EXPECT_EQ(vx.value_.decimal32.value, static_cast<i32>(i));
    }

    EXPECT_EQ(col_decimal32.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal32.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal32T decimal32(static_cast<i32>(i));
        Value v = Value::MakeDecimal32(decimal32, decimal32_info);
        col_decimal32.AppendValue(v);
        Value vx = col_decimal32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal32);
        EXPECT_EQ(vx.value_.decimal32.value, static_cast<i32>(i));
        EXPECT_THROW(col_decimal32.GetValue(i + 1), TypeException);
    }

    col_decimal32.Reset();
    EXPECT_EQ(col_decimal32.capacity(), 0);
    EXPECT_EQ(col_decimal32.tail_index_, 0);
    EXPECT_NE(col_decimal32.buffer_, nullptr);
    EXPECT_NE(col_decimal32.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal32.initialized, false);

    // ====
    col_decimal32.Initialize();
    EXPECT_THROW(col_decimal32.SetDataType(DataType(LogicalType::kDecimal32)), TypeException);
    EXPECT_THROW(col_decimal32.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal32.Size(), 0);
    EXPECT_THROW(col_decimal32.ToString(), TypeException);
    EXPECT_THROW(col_decimal32.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal32.tail_index_, 0);
    EXPECT_EQ(col_decimal32.data_type_size_, 4);
    EXPECT_NE(col_decimal32.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal32.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal32.data_type(), data_type);
    EXPECT_EQ(col_decimal32.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal32.buffer_, nullptr);
    EXPECT_NE(col_decimal32.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal32.initialized);
    col_decimal32.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_decimal32.data_ptr_;
    EXPECT_EQ(col_decimal32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal32.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal32T decimal32(static_cast<i32>(i));
        Value v = Value::MakeDecimal32(decimal32, decimal32_info);
        col_decimal32.AppendValue(v);
        Value vx = col_decimal32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal32);
        EXPECT_EQ(vx.value_.decimal32.value, static_cast<i32>(i));
        EXPECT_THROW(col_decimal32.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorDecimalTest, flat_decimal64) {
    using namespace infinity;

    auto decimal64_info = Decimal64Info::Make(18, 18);
    DataType data_type(LogicalType::kDecimal64, decimal64_info);
    ColumnVector col_decimal64(data_type, ColumnVectorType::kFlat);
    col_decimal64.Initialize();

    EXPECT_THROW(col_decimal64.SetDataType(DataType(LogicalType::kDecimal64)), TypeException);
    EXPECT_THROW(col_decimal64.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal64.Size(), 0);
    EXPECT_THROW(col_decimal64.ToString(), TypeException);
    EXPECT_THROW(col_decimal64.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal64.tail_index_, 0);
    EXPECT_EQ(col_decimal64.data_type_size_, 8);
    EXPECT_NE(col_decimal64.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal64.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal64.data_type(), data_type);
    EXPECT_EQ(col_decimal64.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal64.buffer_, nullptr);
    EXPECT_NE(col_decimal64.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal64.initialized);
    col_decimal64.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_decimal64.data_ptr_;
    EXPECT_EQ(col_decimal64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal64.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal64T decimal64(static_cast<i64>(i));
        Value v = Value::MakeDecimal64(decimal64, decimal64_info);
        col_decimal64.AppendValue(v);
        Value vx = col_decimal64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal64);
        EXPECT_EQ(vx.value_.decimal64.value, static_cast<i64>(i));
        EXPECT_THROW(col_decimal64.GetValue(i + 1), TypeException);
    }

    col_decimal64.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_decimal64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal64);
        EXPECT_EQ(vx.value_.decimal64.value, static_cast<i64>(i));
    }

    EXPECT_EQ(col_decimal64.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal64.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal64T decimal64(static_cast<i64>(i));
        Value v = Value::MakeDecimal64(decimal64, decimal64_info);
        col_decimal64.AppendValue(v);
        Value vx = col_decimal64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal64);
        EXPECT_EQ(vx.value_.decimal64.value, static_cast<i64>(i));
        EXPECT_THROW(col_decimal64.GetValue(i + 1), TypeException);
    }

    col_decimal64.Reset();
    EXPECT_EQ(col_decimal64.capacity(), 0);
    EXPECT_EQ(col_decimal64.tail_index_, 0);
    EXPECT_NE(col_decimal64.buffer_, nullptr);
    EXPECT_NE(col_decimal64.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal64.initialized, false);

    // ====
    col_decimal64.Initialize();
    EXPECT_THROW(col_decimal64.SetDataType(DataType(LogicalType::kDecimal64)), TypeException);
    EXPECT_THROW(col_decimal64.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal64.Size(), 0);
    EXPECT_THROW(col_decimal64.ToString(), TypeException);
    EXPECT_THROW(col_decimal64.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal64.tail_index_, 0);
    EXPECT_EQ(col_decimal64.data_type_size_, 8);
    EXPECT_NE(col_decimal64.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal64.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal64.data_type(), data_type);
    EXPECT_EQ(col_decimal64.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal64.buffer_, nullptr);
    EXPECT_NE(col_decimal64.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal64.initialized);
    col_decimal64.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_decimal64.data_ptr_;
    EXPECT_EQ(col_decimal64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal64.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal64T decimal64(static_cast<i64>(i));
        Value v = Value::MakeDecimal64(decimal64, decimal64_info);
        col_decimal64.AppendValue(v);
        Value vx = col_decimal64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal64);
        EXPECT_EQ(vx.value_.decimal64.value, static_cast<i64>(i));
        EXPECT_THROW(col_decimal64.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorDecimalTest, flat_decimal128) {
    using namespace infinity;

    auto decimal128_info = Decimal128Info::Make(38, 38);
    DataType data_type(LogicalType::kDecimal128, decimal128_info);
    ColumnVector col_decimal128(data_type, ColumnVectorType::kFlat);
    col_decimal128.Initialize();

    EXPECT_THROW(col_decimal128.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_decimal128.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal128.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal128.Size(), 0);
    EXPECT_THROW(col_decimal128.ToString(), TypeException);
    EXPECT_THROW(col_decimal128.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal128.tail_index_, 0);
    EXPECT_EQ(col_decimal128.data_type_size_, 16);
    EXPECT_NE(col_decimal128.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal128.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal128.data_type(), data_type);
    EXPECT_EQ(col_decimal128.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal128.buffer_, nullptr);
    EXPECT_NE(col_decimal128.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal128.initialized);
    col_decimal128.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_decimal128.data_ptr_;
    EXPECT_EQ(col_decimal128.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal128.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal128T decimal128(0, static_cast<i64>(i));
        Value v = Value::MakeDecimal128(decimal128, decimal128_info);
        col_decimal128.AppendValue(v);
        Value vx = col_decimal128.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal128);
        EXPECT_EQ(vx.value_.decimal128.lower, static_cast<i64>(i));
        EXPECT_THROW(col_decimal128.GetValue(i + 1), TypeException);
    }

    col_decimal128.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_decimal128.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal128);
        EXPECT_EQ(vx.value_.decimal128.lower, static_cast<i64>(i));
    }

    EXPECT_EQ(col_decimal128.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal128.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal128T decimal128(0, static_cast<i64>(i));
        Value v = Value::MakeDecimal128(decimal128, decimal128_info);
        col_decimal128.AppendValue(v);
        Value vx = col_decimal128.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal128);
        EXPECT_EQ(vx.value_.decimal128.lower, static_cast<i64>(i));
        EXPECT_THROW(col_decimal128.GetValue(i + 1), TypeException);
    }

    col_decimal128.Reset();
    EXPECT_EQ(col_decimal128.capacity(), 0);
    EXPECT_EQ(col_decimal128.tail_index_, 0);
    EXPECT_NE(col_decimal128.buffer_, nullptr);
    EXPECT_NE(col_decimal128.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal128.initialized, false);

    // ====
    col_decimal128.Initialize();
    EXPECT_THROW(col_decimal128.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_decimal128.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_decimal128.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal128.Size(), 0);
    EXPECT_THROW(col_decimal128.ToString(), TypeException);
    EXPECT_THROW(col_decimal128.GetValue(0), TypeException);
    EXPECT_EQ(col_decimal128.tail_index_, 0);
    EXPECT_EQ(col_decimal128.data_type_size_, 16);
    EXPECT_NE(col_decimal128.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal128.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal128.data_type(), data_type);
    EXPECT_EQ(col_decimal128.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_decimal128.buffer_, nullptr);
    EXPECT_NE(col_decimal128.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_decimal128.initialized);
    col_decimal128.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_decimal128.data_ptr_;
    EXPECT_EQ(col_decimal128.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_decimal128.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Decimal128T decimal128(0, static_cast<i64>(i));
        Value v = Value::MakeDecimal128(decimal128, decimal128_info);
        col_decimal128.AppendValue(v);
        Value vx = col_decimal128.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDecimal128);
        EXPECT_EQ(vx.value_.decimal128.lower, static_cast<i64>(i));
        EXPECT_THROW(col_decimal128.GetValue(i + 1), TypeException);
    }
}
