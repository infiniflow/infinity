//
// Created by JinHai on 2022/11/28.
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
#include "common/types/info/varchar_info.h"

class ColumnVectorTest : public BaseTest {
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

TEST_F(ColumnVectorTest, flat_tinyint) {
    using namespace infinity;

    DataType data_type(LogicalType::kTinyInt);
    ColumnVector col_tinyint(data_type, ColumnVectorType::kFlat);
    col_tinyint.Initialize();

    EXPECT_THROW(col_tinyint.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_tinyint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_tinyint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_tinyint.Size(), 0);
    EXPECT_THROW(col_tinyint.ToString(), std::logic_error);
    EXPECT_THROW(col_tinyint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_tinyint.tail_index_, 0);
    EXPECT_EQ(col_tinyint.data_type_size_, 1);
    EXPECT_NE(col_tinyint.data_ptr_, nullptr);
    EXPECT_EQ(col_tinyint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_tinyint.data_type(), data_type);
    EXPECT_EQ(col_tinyint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_tinyint.buffer_, nullptr);
    EXPECT_EQ(col_tinyint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_tinyint.initialized);
    col_tinyint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_tinyint.data_ptr_;
    EXPECT_EQ(col_tinyint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_tinyint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeTinyInt(static_cast<TinyIntT>(i));
        col_tinyint.AppendValue(v);
        Value vx = col_tinyint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(i));
        EXPECT_THROW(col_tinyint.GetValue(i + 1), std::logic_error);
    }

    col_tinyint.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_tinyint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(i));
    }
    EXPECT_EQ(col_tinyint.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_tinyint.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeTinyInt(static_cast<TinyIntT>(i));
        col_tinyint.AppendValue(v);
        Value vx = col_tinyint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(i));
        EXPECT_THROW(col_tinyint.GetValue(i + 1), std::logic_error);
    }

    col_tinyint.Reset();
    EXPECT_EQ(col_tinyint.capacity(), 0);
    EXPECT_EQ(col_tinyint.tail_index_, 0);
//    EXPECT_EQ(col_tinyint.data_type_size_, 0);
    EXPECT_EQ(col_tinyint.buffer_, nullptr);
    EXPECT_EQ(col_tinyint.data_ptr_, nullptr);
    EXPECT_EQ(col_tinyint.initialized, false);
//    EXPECT_EQ(col_tinyint.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_tinyint.vector_type(), ColumnVectorType::kInvalid);

    // ====
//    EXPECT_THROW(col_tinyint.Initialize(), std::logic_error);
//    col_tinyint.SetDataType(DataType(LogicalType::kTinyInt));
//    EXPECT_THROW(col_tinyint.Initialize(), std::logic_error);
//    col_tinyint.SetVectorType(ColumnVectorType::kFlat);
    col_tinyint.Initialize();
    EXPECT_THROW(col_tinyint.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_tinyint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_tinyint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_tinyint.Size(), 0);
    EXPECT_THROW(col_tinyint.ToString(), std::logic_error);
    EXPECT_THROW(col_tinyint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_tinyint.tail_index_, 0);
    EXPECT_EQ(col_tinyint.data_type_size_, 1);
    EXPECT_NE(col_tinyint.data_ptr_, nullptr);
    EXPECT_EQ(col_tinyint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_tinyint.data_type(), data_type);
    EXPECT_EQ(col_tinyint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_tinyint.buffer_, nullptr);
    EXPECT_EQ(col_tinyint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_tinyint.initialized);
    col_tinyint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_tinyint.data_ptr_;
    EXPECT_EQ(col_tinyint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_tinyint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeTinyInt(static_cast<TinyIntT>(i));
        col_tinyint.AppendValue(v);
        Value vx = col_tinyint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(i));
        EXPECT_THROW(col_tinyint.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_smallint) {
    using namespace infinity;

    DataType data_type(LogicalType::kSmallInt);
    ColumnVector col_smallint(data_type, ColumnVectorType::kFlat);
    col_smallint.Initialize();

    EXPECT_THROW(col_smallint.SetDataType(DataType(LogicalType::kSmallInt)), std::logic_error);
    EXPECT_THROW(col_smallint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_smallint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_smallint.Size(), 0);
    EXPECT_THROW(col_smallint.ToString(), std::logic_error);
    EXPECT_THROW(col_smallint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_smallint.tail_index_, 0);
    EXPECT_EQ(col_smallint.data_type_size_, 2);
    EXPECT_NE(col_smallint.data_ptr_, nullptr);
    EXPECT_EQ(col_smallint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_smallint.data_type(), data_type);
    EXPECT_EQ(col_smallint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_smallint.buffer_, nullptr);
    EXPECT_EQ(col_smallint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_smallint.initialized);
    col_smallint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_smallint.data_ptr_;
    EXPECT_EQ(col_smallint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_smallint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeSmallInt(static_cast<SmallIntT>(i));
        col_smallint.AppendValue(v);
        Value vx = col_smallint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
        EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(i));
        EXPECT_THROW(col_smallint.GetValue(i + 1), std::logic_error);
    }

    col_smallint.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_smallint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
        EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(i));
    }
    EXPECT_EQ(col_smallint.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_smallint.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeSmallInt(static_cast<SmallIntT>(i));
        col_smallint.AppendValue(v);
        Value vx = col_smallint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
        EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(i));
        EXPECT_THROW(col_smallint.GetValue(i + 1), std::logic_error);
    }

    col_smallint.Reset();
    EXPECT_EQ(col_smallint.capacity(), 0);
    EXPECT_EQ(col_smallint.tail_index_, 0);
//    EXPECT_EQ(col_smallint.data_type_size_, 0);
    EXPECT_EQ(col_smallint.buffer_, nullptr);
    EXPECT_EQ(col_smallint.data_ptr_, nullptr);
    EXPECT_EQ(col_smallint.initialized, false);
//    EXPECT_EQ(col_smallint.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_smallint.vector_type(), ColumnVectorType::kInvalid);

    // ====
//    EXPECT_THROW(col_smallint.Initialize(), std::logic_error);
//    col_smallint.SetDataType(DataType(LogicalType::kSmallInt));
//    EXPECT_THROW(col_smallint.Initialize(), std::logic_error);
//    col_smallint.SetVectorType(ColumnVectorType::kFlat);
    col_smallint.Initialize();
    EXPECT_THROW(col_smallint.SetDataType(DataType(LogicalType::kSmallInt)), std::logic_error);
    EXPECT_THROW(col_smallint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_smallint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_smallint.Size(), 0);
    EXPECT_THROW(col_smallint.ToString(), std::logic_error);
    EXPECT_THROW(col_smallint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_smallint.tail_index_, 0);
    EXPECT_EQ(col_smallint.data_type_size_, 2);
    EXPECT_NE(col_smallint.data_ptr_, nullptr);
    EXPECT_EQ(col_smallint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_smallint.data_type(), data_type);
    EXPECT_EQ(col_smallint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_smallint.buffer_, nullptr);
    EXPECT_EQ(col_smallint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_smallint.initialized);
    col_smallint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_smallint.data_ptr_;
    EXPECT_EQ(col_smallint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_smallint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeSmallInt(static_cast<SmallIntT>(i));
        col_smallint.AppendValue(v);
        Value vx = col_smallint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kSmallInt);
        EXPECT_EQ(vx.value_.small_int, static_cast<SmallIntT>(i));
        EXPECT_THROW(col_smallint.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_int) {
    using namespace infinity;

    DataType data_type(LogicalType::kInteger);
    ColumnVector col_int(data_type, ColumnVectorType::kFlat);
    col_int.Initialize();

    EXPECT_THROW(col_int.SetDataType(DataType(LogicalType::kInteger)), std::logic_error);
    EXPECT_THROW(col_int.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_int.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_int.Size(), 0);
    EXPECT_THROW(col_int.ToString(), std::logic_error);
    EXPECT_THROW(col_int.GetValue(0), std::logic_error);
    EXPECT_EQ(col_int.tail_index_, 0);
    EXPECT_EQ(col_int.data_type_size_, 4);
    EXPECT_NE(col_int.data_ptr_, nullptr);
    EXPECT_EQ(col_int.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_int.data_type(), data_type);
    EXPECT_EQ(col_int.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_int.buffer_, nullptr);
    EXPECT_EQ(col_int.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_int.initialized);
    col_int.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_int.data_ptr_;
    EXPECT_EQ(col_int.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_int.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeInt(static_cast<IntegerT>(i));
        col_int.AppendValue(v);
        Value vx = col_int.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
        EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(i));
        EXPECT_THROW(col_int.GetValue(i + 1), std::logic_error);
    }

    col_int.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_int.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
        EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(i));
    }
    EXPECT_EQ(col_int.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_int.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeInt(static_cast<IntegerT>(i));
        col_int.AppendValue(v);
        Value vx = col_int.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
        EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(i));
        EXPECT_THROW(col_int.GetValue(i + 1), std::logic_error);
    }

    col_int.Reset();
    EXPECT_EQ(col_int.capacity(), 0);
    EXPECT_EQ(col_int.tail_index_, 0);
//    EXPECT_EQ(col_int.data_type_size_, 0);
    EXPECT_EQ(col_int.buffer_, nullptr);
    EXPECT_EQ(col_int.data_ptr_, nullptr);
    EXPECT_EQ(col_int.initialized, false);
//    EXPECT_EQ(col_int.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_int.vector_type(), ColumnVectorType::kInvalid);

    // ====
//    EXPECT_THROW(col_int.Initialize(), std::logic_error);
//    col_int.SetDataType(DataType(LogicalType::kInteger));
//    EXPECT_THROW(col_int.Initialize(), std::logic_error);
//    col_int.SetVectorType(ColumnVectorType::kFlat);
    col_int.Initialize();
    EXPECT_THROW(col_int.SetDataType(DataType(LogicalType::kInteger)), std::logic_error);
    EXPECT_THROW(col_int.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_int.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_int.Size(), 0);
    EXPECT_THROW(col_int.ToString(), std::logic_error);
    EXPECT_THROW(col_int.GetValue(0), std::logic_error);
    EXPECT_EQ(col_int.tail_index_, 0);
    EXPECT_EQ(col_int.data_type_size_, 4);
    EXPECT_NE(col_int.data_ptr_, nullptr);
    EXPECT_EQ(col_int.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_int.data_type(), data_type);
    EXPECT_EQ(col_int.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_int.buffer_, nullptr);
    EXPECT_EQ(col_int.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_int.initialized);
    col_int.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_int.data_ptr_;
    EXPECT_EQ(col_int.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_int.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeInt(static_cast<IntegerT>(i));
        col_int.AppendValue(v);
        Value vx = col_int.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInteger);
        EXPECT_EQ(vx.value_.integer, static_cast<IntegerT>(i));
        EXPECT_THROW(col_int.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_bigint) {
    using namespace infinity;

    DataType data_type(LogicalType::kBigInt);
    ColumnVector col_bigint(data_type, ColumnVectorType::kFlat);
    col_bigint.Initialize();

    EXPECT_THROW(col_bigint.SetDataType(DataType(LogicalType::kBigInt)), std::logic_error);
    EXPECT_THROW(col_bigint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_bigint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bigint.Size(), 0);
    EXPECT_THROW(col_bigint.ToString(), std::logic_error);
    EXPECT_THROW(col_bigint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_bigint.tail_index_, 0);
    EXPECT_EQ(col_bigint.data_type_size_, 8);
    EXPECT_NE(col_bigint.data_ptr_, nullptr);
    EXPECT_EQ(col_bigint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_bigint.data_type(), data_type);
    EXPECT_EQ(col_bigint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_bigint.buffer_, nullptr);
    EXPECT_EQ(col_bigint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_bigint.initialized);
    col_bigint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_bigint.data_ptr_;
    EXPECT_EQ(col_bigint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_bigint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBigInt(static_cast<BigIntT>(i));
        col_bigint.AppendValue(v);
        Value vx = col_bigint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
        EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(i));
        EXPECT_THROW(col_bigint.GetValue(i + 1), std::logic_error);
    }

    col_bigint.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_bigint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
        EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(i));
    }
    EXPECT_EQ(col_bigint.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bigint.capacity(), 2* DEFAULT_VECTOR_SIZE);
//    return ;
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBigInt(static_cast<BigIntT>(i));
        col_bigint.AppendValue(v);
        Value vx = col_bigint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
        EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(i));
        EXPECT_THROW(col_bigint.GetValue(i + 1), std::logic_error);
    }

    col_bigint.Reset();
    EXPECT_EQ(col_bigint.capacity(), 0);
    EXPECT_EQ(col_bigint.tail_index_, 0);
//    EXPECT_EQ(col_bigint.data_type_size_, 0);
    EXPECT_EQ(col_bigint.buffer_, nullptr);
    EXPECT_EQ(col_bigint.data_ptr_, nullptr);
    EXPECT_EQ(col_bigint.initialized, false);
//    EXPECT_EQ(col_bigint.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_bigint.vector_type(), ColumnVectorType::kInvalid);

    // ====
//    EXPECT_THROW(col_bigint.Initialize(), std::logic_error);
//    col_bigint.SetDataType(DataType(LogicalType::kBigInt));
//    EXPECT_THROW(col_bigint.Initialize(), std::logic_error);
//    col_bigint.SetVectorType(ColumnVectorType::kFlat);
    col_bigint.Initialize();
    EXPECT_THROW(col_bigint.SetDataType(DataType(LogicalType::kBigInt)), std::logic_error);
    EXPECT_THROW(col_bigint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_bigint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bigint.Size(), 0);
    EXPECT_THROW(col_bigint.ToString(), std::logic_error);
    EXPECT_THROW(col_bigint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_bigint.tail_index_, 0);
    EXPECT_EQ(col_bigint.data_type_size_, 8);
    EXPECT_NE(col_bigint.data_ptr_, nullptr);
    EXPECT_EQ(col_bigint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_bigint.data_type(), data_type);
    EXPECT_EQ(col_bigint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_bigint.buffer_, nullptr);
    EXPECT_EQ(col_bigint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_bigint.initialized);
    col_bigint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_bigint.data_ptr_;
    EXPECT_EQ(col_bigint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_bigint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBigInt(static_cast<BigIntT>(i));
        col_bigint.AppendValue(v);
        Value vx = col_bigint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBigInt);
        EXPECT_EQ(vx.value_.big_int, static_cast<BigIntT>(i));
        EXPECT_THROW(col_bigint.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_hugeint) {
    using namespace infinity;

    DataType data_type(LogicalType::kHugeInt);
    ColumnVector col_hugeint(data_type, ColumnVectorType::kFlat);
    col_hugeint.Initialize();

    EXPECT_THROW(col_hugeint.SetDataType(DataType(LogicalType::kHugeInt)), std::logic_error);
    EXPECT_THROW(col_hugeint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_hugeint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_hugeint.Size(), 0);
    EXPECT_THROW(col_hugeint.ToString(), std::logic_error);
    EXPECT_THROW(col_hugeint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_hugeint.tail_index_, 0);
    EXPECT_EQ(col_hugeint.data_type_size_, 16);
    EXPECT_NE(col_hugeint.data_ptr_, nullptr);
    EXPECT_EQ(col_hugeint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_hugeint.data_type(), data_type);
    EXPECT_EQ(col_hugeint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_hugeint.buffer_, nullptr);
    EXPECT_EQ(col_hugeint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_hugeint.initialized);
    col_hugeint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_hugeint.data_ptr_;
    EXPECT_EQ(col_hugeint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_hugeint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        HugeIntT input(0, i);
        Value v = Value::MakeHugeInt(input);
        col_hugeint.AppendValue(v);
        Value vx = col_hugeint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
        EXPECT_EQ(vx.value_.huge_int, input);
        EXPECT_THROW(col_hugeint.GetValue(i + 1), std::logic_error);
    }

    col_hugeint.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_hugeint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
        EXPECT_EQ(vx.value_.huge_int, HugeIntT(0, i));
    }
    EXPECT_EQ(col_hugeint.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_hugeint.capacity(), 2* DEFAULT_VECTOR_SIZE);
//    return ;
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        HugeIntT input(0, i);
        Value v = Value::MakeHugeInt(input);
        col_hugeint.AppendValue(v);
        Value vx = col_hugeint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
        EXPECT_EQ(vx.value_.huge_int, input);
        EXPECT_THROW(col_hugeint.GetValue(i + 1), std::logic_error);
    }

    col_hugeint.Reset();
    EXPECT_EQ(col_hugeint.capacity(), 0);
    EXPECT_EQ(col_hugeint.tail_index_, 0);
//    EXPECT_EQ(col_hugeint.data_type_size_, 0);
    EXPECT_EQ(col_hugeint.buffer_, nullptr);
    EXPECT_EQ(col_hugeint.data_ptr_, nullptr);
    EXPECT_EQ(col_hugeint.initialized, false);
//    EXPECT_EQ(col_hugeint.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_hugeint.vector_type(), ColumnVectorType::kInvalid);
//
    // ====
//    EXPECT_THROW(col_hugeint.Initialize(), std::logic_error);
//    col_hugeint.SetDataType(DataType(LogicalType::kHugeInt));
//    EXPECT_THROW(col_hugeint.Initialize(), std::logic_error);
//    col_hugeint.SetVectorType(ColumnVectorType::kFlat);
    col_hugeint.Initialize();
    EXPECT_THROW(col_hugeint.SetDataType(DataType(LogicalType::kHugeInt)), std::logic_error);
    EXPECT_THROW(col_hugeint.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_hugeint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_hugeint.Size(), 0);
    EXPECT_THROW(col_hugeint.ToString(), std::logic_error);
    EXPECT_THROW(col_hugeint.GetValue(0), std::logic_error);
    EXPECT_EQ(col_hugeint.tail_index_, 0);
    EXPECT_EQ(col_hugeint.data_type_size_, 16);
    EXPECT_NE(col_hugeint.data_ptr_, nullptr);
    EXPECT_EQ(col_hugeint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_hugeint.data_type(), data_type);
    EXPECT_EQ(col_hugeint.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_hugeint.buffer_, nullptr);
    EXPECT_EQ(col_hugeint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_hugeint.initialized);
    col_hugeint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_hugeint.data_ptr_;
    EXPECT_EQ(col_hugeint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_hugeint.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        HugeIntT input(0, i);
        Value v = Value::MakeHugeInt(input);
        col_hugeint.AppendValue(v);
        Value vx = col_hugeint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kHugeInt);
        EXPECT_EQ(vx.value_.huge_int, input);
        EXPECT_THROW(col_hugeint.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_float) {
    using namespace infinity;

    DataType data_type(LogicalType::kFloat);
    ColumnVector col_float(data_type, ColumnVectorType::kFlat);
    col_float.Initialize();

    EXPECT_THROW(col_float.SetDataType(DataType(LogicalType::kFloat)), std::logic_error);
    EXPECT_THROW(col_float.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_float.Size(), 0);
    EXPECT_THROW(col_float.ToString(), std::logic_error);
    EXPECT_THROW(col_float.GetValue(0), std::logic_error);
    EXPECT_EQ(col_float.tail_index_, 0);
    EXPECT_EQ(col_float.data_type_size_, 4);
    EXPECT_NE(col_float.data_ptr_, nullptr);
    EXPECT_EQ(col_float.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_float.data_type(), data_type);
    EXPECT_EQ(col_float.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_float.buffer_, nullptr);
    EXPECT_EQ(col_float.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_float.initialized);
    col_float.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_float.data_ptr_;
    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_float.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        col_float.AppendValue(v);
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(col_float.GetValue(i + 1), std::logic_error);
    }

    col_float.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
    }
    EXPECT_EQ(col_float.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_float.capacity(), 2* DEFAULT_VECTOR_SIZE);
//    return ;
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        col_float.AppendValue(v);
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(col_float.GetValue(i + 1), std::logic_error);
    }

    col_float.Reset();
    EXPECT_EQ(col_float.capacity(), 0);
    EXPECT_EQ(col_float.tail_index_, 0);
//    EXPECT_EQ(col_float.data_type_size_, 0);
    EXPECT_EQ(col_float.buffer_, nullptr);
    EXPECT_EQ(col_float.data_ptr_, nullptr);
    EXPECT_EQ(col_float.initialized, false);
//    EXPECT_EQ(col_float.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_float.vector_type(), ColumnVectorType::kInvalid);

    // ====
//    EXPECT_THROW(col_float.Initialize(), std::logic_error);
//    col_float.SetDataType(DataType(LogicalType::kFloat));
//    EXPECT_THROW(col_float.Initialize(), std::logic_error);
//    col_float.SetVectorType(ColumnVectorType::kFlat);
    col_float.Initialize();
    EXPECT_THROW(col_float.SetDataType(DataType(LogicalType::kFloat)), std::logic_error);
    EXPECT_THROW(col_float.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_float.Size(), 0);
    EXPECT_THROW(col_float.ToString(), std::logic_error);
    EXPECT_THROW(col_float.GetValue(0), std::logic_error);
    EXPECT_EQ(col_float.tail_index_, 0);
    EXPECT_EQ(col_float.data_type_size_, 4);
    EXPECT_NE(col_float.data_ptr_, nullptr);
    EXPECT_EQ(col_float.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_float.data_type(), data_type);
    EXPECT_EQ(col_float.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_float.buffer_, nullptr);
    EXPECT_EQ(col_float.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_float.initialized);
    col_float.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_float.data_ptr_;
    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_float.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        col_float.AppendValue(v);
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(col_float.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_double) {
    using namespace infinity;

    DataType data_type(LogicalType::kDouble);
    ColumnVector col_double(data_type, ColumnVectorType::kFlat);
    col_double.Initialize();

    EXPECT_THROW(col_double.SetDataType(DataType(LogicalType::kDouble)), std::logic_error);
    EXPECT_THROW(col_double.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_double.Size(), 0);
    EXPECT_THROW(col_double.ToString(), std::logic_error);
    EXPECT_THROW(col_double.GetValue(0), std::logic_error);
    EXPECT_EQ(col_double.tail_index_, 0);
    EXPECT_EQ(col_double.data_type_size_, 8);
    EXPECT_NE(col_double.data_ptr_, nullptr);
    EXPECT_EQ(col_double.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_double.data_type(), data_type);
    EXPECT_EQ(col_double.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_double.buffer_, nullptr);
    EXPECT_EQ(col_double.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_double.initialized);
    col_double.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_double.data_ptr_;
    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_double.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        col_double.AppendValue(v);
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
    }

    col_double.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
    }
    EXPECT_EQ(col_double.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_double.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        col_double.AppendValue(v);
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
    }

    col_double.Reset();
    EXPECT_EQ(col_double.capacity(), 0);
    EXPECT_EQ(col_double.tail_index_, 0);
    EXPECT_EQ(col_double.buffer_, nullptr);
    EXPECT_EQ(col_double.data_ptr_, nullptr);
    EXPECT_EQ(col_double.initialized, false);

    // ====
    col_double.Initialize();
    EXPECT_THROW(col_double.SetDataType(DataType(LogicalType::kDouble)), std::logic_error);
    EXPECT_THROW(col_double.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_double.Size(), 0);
    EXPECT_THROW(col_double.ToString(), std::logic_error);
    EXPECT_THROW(col_double.GetValue(0), std::logic_error);
    EXPECT_EQ(col_double.tail_index_, 0);
    EXPECT_EQ(col_double.data_type_size_, 8);
    EXPECT_NE(col_double.data_ptr_, nullptr);
    EXPECT_EQ(col_double.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_double.data_type(), data_type);
    EXPECT_EQ(col_double.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_double.buffer_, nullptr);
    EXPECT_EQ(col_double.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_double.initialized);
    col_double.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_double.data_ptr_;
    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_double.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        col_double.AppendValue(v);
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
    }
}


TEST_F(ColumnVectorTest, flat_decimal16) {
    using namespace infinity;

    auto decimal16_info = Decimal16Info::Make(4, 4);
    DataType data_type(LogicalType::kDecimal16, decimal16_info);
    ColumnVector col_decimal16(data_type, ColumnVectorType::kFlat);
    col_decimal16.Initialize();

    EXPECT_THROW(col_decimal16.SetDataType(DataType(LogicalType::kDecimal16)), std::logic_error);
    EXPECT_THROW(col_decimal16.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal16.Size(), 0);
    EXPECT_THROW(col_decimal16.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal16.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal16.tail_index_, 0);
    EXPECT_EQ(col_decimal16.data_type_size_, 2);
    EXPECT_NE(col_decimal16.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal16.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal16.data_type(), data_type);
    EXPECT_EQ(col_decimal16.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal16.buffer_, nullptr);
    EXPECT_EQ(col_decimal16.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal16.value, static_cast<i16>(i));
        EXPECT_THROW(col_decimal16.GetValue(i + 1), std::logic_error);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal16.value, static_cast<i16>(i));
        EXPECT_THROW(col_decimal16.GetValue(i + 1), std::logic_error);
    }

    col_decimal16.Reset();
    EXPECT_EQ(col_decimal16.capacity(), 0);
    EXPECT_EQ(col_decimal16.tail_index_, 0);
    EXPECT_EQ(col_decimal16.buffer_, nullptr);
    EXPECT_EQ(col_decimal16.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal16.initialized, false);

    // ====
    col_decimal16.Initialize();
    EXPECT_THROW(col_decimal16.SetDataType(DataType(LogicalType::kDecimal16)), std::logic_error);
    EXPECT_THROW(col_decimal16.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal16.Size(), 0);
    EXPECT_THROW(col_decimal16.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal16.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal16.tail_index_, 0);
    EXPECT_EQ(col_decimal16.data_type_size_, 2);
    EXPECT_NE(col_decimal16.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal16.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal16.data_type(), data_type);
    EXPECT_EQ(col_decimal16.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal16.buffer_, nullptr);
    EXPECT_EQ(col_decimal16.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal16.value, static_cast<i16>(i));
        EXPECT_THROW(col_decimal16.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_decimal32) {
    using namespace infinity;

    auto decimal32_info = Decimal32Info::Make(9, 9);
    DataType data_type(LogicalType::kDecimal32, decimal32_info);
    ColumnVector col_decimal32(data_type, ColumnVectorType::kFlat);
    col_decimal32.Initialize();

    EXPECT_THROW(col_decimal32.SetDataType(DataType(LogicalType::kDecimal32)), std::logic_error);
    EXPECT_THROW(col_decimal32.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal32.Size(), 0);
    EXPECT_THROW(col_decimal32.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal32.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal32.tail_index_, 0);
    EXPECT_EQ(col_decimal32.data_type_size_, 4);
    EXPECT_NE(col_decimal32.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal32.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal32.data_type(), data_type);
    EXPECT_EQ(col_decimal32.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal32.buffer_, nullptr);
    EXPECT_EQ(col_decimal32.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal32.value, static_cast<i32>(i));
        EXPECT_THROW(col_decimal32.GetValue(i + 1), std::logic_error);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal32.value, static_cast<i32>(i));
        EXPECT_THROW(col_decimal32.GetValue(i + 1), std::logic_error);
    }

    col_decimal32.Reset();
    EXPECT_EQ(col_decimal32.capacity(), 0);
    EXPECT_EQ(col_decimal32.tail_index_, 0);
    EXPECT_EQ(col_decimal32.buffer_, nullptr);
    EXPECT_EQ(col_decimal32.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal32.initialized, false);

    // ====
    col_decimal32.Initialize();
    EXPECT_THROW(col_decimal32.SetDataType(DataType(LogicalType::kDecimal32)), std::logic_error);
    EXPECT_THROW(col_decimal32.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal32.Size(), 0);
    EXPECT_THROW(col_decimal32.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal32.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal32.tail_index_, 0);
    EXPECT_EQ(col_decimal32.data_type_size_, 4);
    EXPECT_NE(col_decimal32.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal32.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal32.data_type(), data_type);
    EXPECT_EQ(col_decimal32.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal32.buffer_, nullptr);
    EXPECT_EQ(col_decimal32.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal32.value, static_cast<i32>(i));
        EXPECT_THROW(col_decimal32.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_decimal64) {
    using namespace infinity;

    auto decimal64_info = Decimal64Info::Make(18, 18);
    DataType data_type(LogicalType::kDecimal64, decimal64_info);
    ColumnVector col_decimal64(data_type, ColumnVectorType::kFlat);
    col_decimal64.Initialize();

    EXPECT_THROW(col_decimal64.SetDataType(DataType(LogicalType::kDecimal64)), std::logic_error);
    EXPECT_THROW(col_decimal64.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal64.Size(), 0);
    EXPECT_THROW(col_decimal64.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal64.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal64.tail_index_, 0);
    EXPECT_EQ(col_decimal64.data_type_size_, 8);
    EXPECT_NE(col_decimal64.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal64.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal64.data_type(), data_type);
    EXPECT_EQ(col_decimal64.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal64.buffer_, nullptr);
    EXPECT_EQ(col_decimal64.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal64.value, static_cast<i64>(i));
        EXPECT_THROW(col_decimal64.GetValue(i + 1), std::logic_error);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal64.value, static_cast<i64>(i));
        EXPECT_THROW(col_decimal64.GetValue(i + 1), std::logic_error);
    }

    col_decimal64.Reset();
    EXPECT_EQ(col_decimal64.capacity(), 0);
    EXPECT_EQ(col_decimal64.tail_index_, 0);
    EXPECT_EQ(col_decimal64.buffer_, nullptr);
    EXPECT_EQ(col_decimal64.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal64.initialized, false);

    // ====
    col_decimal64.Initialize();
    EXPECT_THROW(col_decimal64.SetDataType(DataType(LogicalType::kDecimal64)), std::logic_error);
    EXPECT_THROW(col_decimal64.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal64.Size(), 0);
    EXPECT_THROW(col_decimal64.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal64.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal64.tail_index_, 0);
    EXPECT_EQ(col_decimal64.data_type_size_, 8);
    EXPECT_NE(col_decimal64.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal64.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal64.data_type(), data_type);
    EXPECT_EQ(col_decimal64.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal64.buffer_, nullptr);
    EXPECT_EQ(col_decimal64.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal64.value, static_cast<i64>(i));
        EXPECT_THROW(col_decimal64.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_decimal128) {
    using namespace infinity;

    auto decimal128_info = Decimal128Info::Make(38, 38);
    DataType data_type(LogicalType::kDecimal128, decimal128_info);
    ColumnVector col_decimal128(data_type, ColumnVectorType::kFlat);
    col_decimal128.Initialize();

    EXPECT_THROW(col_decimal128.SetDataType(DataType(LogicalType::kDecimal128)), std::logic_error);
    EXPECT_THROW(col_decimal128.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal128.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal128.Size(), 0);
    EXPECT_THROW(col_decimal128.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal128.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal128.tail_index_, 0);
    EXPECT_EQ(col_decimal128.data_type_size_, 16);
    EXPECT_NE(col_decimal128.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal128.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal128.data_type(), data_type);
    EXPECT_EQ(col_decimal128.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal128.buffer_, nullptr);
    EXPECT_EQ(col_decimal128.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal128.lower, static_cast<i64>(i));
        EXPECT_THROW(col_decimal128.GetValue(i + 1), std::logic_error);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal128.lower, static_cast<i64>(i));
        EXPECT_THROW(col_decimal128.GetValue(i + 1), std::logic_error);
    }

    col_decimal128.Reset();
    EXPECT_EQ(col_decimal128.capacity(), 0);
    EXPECT_EQ(col_decimal128.tail_index_, 0);
    EXPECT_EQ(col_decimal128.buffer_, nullptr);
    EXPECT_EQ(col_decimal128.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal128.initialized, false);

    // ====
    col_decimal128.Initialize();
    EXPECT_THROW(col_decimal128.SetDataType(DataType(LogicalType::kDecimal128)), std::logic_error);
    EXPECT_THROW(col_decimal128.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_decimal128.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_decimal128.Size(), 0);
    EXPECT_THROW(col_decimal128.ToString(), std::logic_error);
    EXPECT_THROW(col_decimal128.GetValue(0), std::logic_error);
    EXPECT_EQ(col_decimal128.tail_index_, 0);
    EXPECT_EQ(col_decimal128.data_type_size_, 16);
    EXPECT_NE(col_decimal128.data_ptr_, nullptr);
    EXPECT_EQ(col_decimal128.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_decimal128.data_type(), data_type);
    EXPECT_EQ(col_decimal128.buffer_->buffer_type_, VectorBufferType::kNormal);

    EXPECT_NE(col_decimal128.buffer_, nullptr);
    EXPECT_EQ(col_decimal128.nulls_ptr_, nullptr);
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
        EXPECT_FLOAT_EQ(vx.value_.decimal128.lower, static_cast<i64>(i));
        EXPECT_THROW(col_decimal128.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_inline_varchar) {
    using namespace infinity;

    auto varchar_info = VarcharInfo::Make(65);
    DataType data_type(LogicalType::kVarchar, varchar_info);
    ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
    col_varchar.Initialize();

    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kVarchar)), std::logic_error);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), std::logic_error);
    EXPECT_THROW(col_varchar.GetValue(0), std::logic_error);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kString);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_EQ(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "hello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), std::logic_error);
    }

    col_varchar.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        String s = "hello" + std::to_string(i);

        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
    }

    EXPECT_EQ(col_varchar.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {

        String s = "hello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), std::logic_error);
    }

    col_varchar.Reset();
    EXPECT_EQ(col_varchar.capacity(), 0);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.buffer_, nullptr);
    EXPECT_EQ(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.initialized, false);

    // ====
    col_varchar.Initialize();
    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kDecimal128)), std::logic_error);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), std::logic_error);
    EXPECT_THROW(col_varchar.GetValue(0), std::logic_error);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kString);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_EQ(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "hello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), std::logic_error);
    }
}
