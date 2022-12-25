//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ColumnVectorIntegerTest : public BaseTest {
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

TEST_F(ColumnVectorIntegerTest, flat_tinyint) {
    using namespace infinity;

    DataType data_type(LogicalType::kTinyInt);
    ColumnVector col_tinyint(data_type, ColumnVectorType::kFlat);
    col_tinyint.Initialize();

    EXPECT_THROW(col_tinyint.SetDataType(DataType(LogicalType::kTinyInt)), TypeException);
    EXPECT_THROW(col_tinyint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_tinyint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_tinyint.Size(), 0);
    EXPECT_THROW(col_tinyint.ToString(), TypeException);
    EXPECT_THROW(col_tinyint.GetValue(0), TypeException);
    EXPECT_EQ(col_tinyint.tail_index_, 0);
    EXPECT_EQ(col_tinyint.data_type_size_, 1);
    EXPECT_NE(col_tinyint.data_ptr_, nullptr);
    EXPECT_EQ(col_tinyint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_tinyint.data_type(), data_type);
    EXPECT_EQ(col_tinyint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_tinyint.buffer_, nullptr);
    EXPECT_NE(col_tinyint.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_tinyint.initialized);
    col_tinyint.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_tinyint.data_ptr_;
    EXPECT_EQ(col_tinyint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_tinyint.data_ptr_);

    {
        Value v = Value::MakeTinyInt(static_cast<TinyIntT>(3));
        EXPECT_THROW(col_tinyint.SetValue(3, v), StorageException);
    }

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeTinyInt(static_cast<TinyIntT>(i));
        col_tinyint.AppendValue(v);
        Value vx = col_tinyint.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(vx.value_.tiny_int, static_cast<TinyIntT>(i));
        EXPECT_THROW(col_tinyint.GetValue(i + 1), TypeException);
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
        EXPECT_THROW(col_tinyint.GetValue(i + 1), TypeException);
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
//    EXPECT_THROW(col_tinyint.Initialize(), TypeException);
//    col_tinyint.SetDataType(DataType(LogicalType::kTinyInt));
//    EXPECT_THROW(col_tinyint.Initialize(), TypeException);
//    col_tinyint.SetVectorType(ColumnVectorType::kFlat);
    col_tinyint.Initialize();
    EXPECT_THROW(col_tinyint.SetDataType(DataType(LogicalType::kTinyInt)), TypeException);
    EXPECT_THROW(col_tinyint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_tinyint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_tinyint.Size(), 0);
    EXPECT_THROW(col_tinyint.ToString(), TypeException);
    EXPECT_THROW(col_tinyint.GetValue(0), TypeException);
    EXPECT_EQ(col_tinyint.tail_index_, 0);
    EXPECT_EQ(col_tinyint.data_type_size_, 1);
    EXPECT_NE(col_tinyint.data_ptr_, nullptr);
    EXPECT_EQ(col_tinyint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_tinyint.data_type(), data_type);
    EXPECT_EQ(col_tinyint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_tinyint.buffer_, nullptr);
    EXPECT_NE(col_tinyint.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_tinyint.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorIntegerTest, flat_smallint) {
    using namespace infinity;

    DataType data_type(LogicalType::kSmallInt);
    ColumnVector col_smallint(data_type, ColumnVectorType::kFlat);
    col_smallint.Initialize();

    EXPECT_THROW(col_smallint.SetDataType(DataType(LogicalType::kSmallInt)), TypeException);
    EXPECT_THROW(col_smallint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_smallint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_smallint.Size(), 0);
    EXPECT_THROW(col_smallint.ToString(), TypeException);
    EXPECT_THROW(col_smallint.GetValue(0), TypeException);
    EXPECT_EQ(col_smallint.tail_index_, 0);
    EXPECT_EQ(col_smallint.data_type_size_, 2);
    EXPECT_NE(col_smallint.data_ptr_, nullptr);
    EXPECT_EQ(col_smallint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_smallint.data_type(), data_type);
    EXPECT_EQ(col_smallint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_smallint.buffer_, nullptr);
    EXPECT_NE(col_smallint.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_smallint.GetValue(i + 1), TypeException);
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
        EXPECT_THROW(col_smallint.GetValue(i + 1), TypeException);
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
//    EXPECT_THROW(col_smallint.Initialize(), TypeException);
//    col_smallint.SetDataType(DataType(LogicalType::kSmallInt));
//    EXPECT_THROW(col_smallint.Initialize(), TypeException);
//    col_smallint.SetVectorType(ColumnVectorType::kFlat);
    col_smallint.Initialize();
    EXPECT_THROW(col_smallint.SetDataType(DataType(LogicalType::kSmallInt)), TypeException);
    EXPECT_THROW(col_smallint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_smallint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_smallint.Size(), 0);
    EXPECT_THROW(col_smallint.ToString(), TypeException);
    EXPECT_THROW(col_smallint.GetValue(0), TypeException);
    EXPECT_EQ(col_smallint.tail_index_, 0);
    EXPECT_EQ(col_smallint.data_type_size_, 2);
    EXPECT_NE(col_smallint.data_ptr_, nullptr);
    EXPECT_EQ(col_smallint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_smallint.data_type(), data_type);
    EXPECT_EQ(col_smallint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_smallint.buffer_, nullptr);
    EXPECT_NE(col_smallint.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_smallint.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorIntegerTest, flat_int) {
    using namespace infinity;

    DataType data_type(LogicalType::kInteger);
    ColumnVector col_int(data_type, ColumnVectorType::kFlat);
    col_int.Initialize();

    EXPECT_THROW(col_int.SetDataType(DataType(LogicalType::kInteger)), TypeException);
    EXPECT_THROW(col_int.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_int.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_int.Size(), 0);
    EXPECT_THROW(col_int.ToString(), TypeException);
    EXPECT_THROW(col_int.GetValue(0), TypeException);
    EXPECT_EQ(col_int.tail_index_, 0);
    EXPECT_EQ(col_int.data_type_size_, 4);
    EXPECT_NE(col_int.data_ptr_, nullptr);
    EXPECT_EQ(col_int.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_int.data_type(), data_type);
    EXPECT_EQ(col_int.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_int.buffer_, nullptr);
    EXPECT_NE(col_int.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_int.GetValue(i + 1), TypeException);
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
        EXPECT_THROW(col_int.GetValue(i + 1), TypeException);
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
//    EXPECT_THROW(col_int.Initialize(), TypeException);
//    col_int.SetDataType(DataType(LogicalType::kInteger));
//    EXPECT_THROW(col_int.Initialize(), TypeException);
//    col_int.SetVectorType(ColumnVectorType::kFlat);
    col_int.Initialize();
    EXPECT_THROW(col_int.SetDataType(DataType(LogicalType::kInteger)), TypeException);
    EXPECT_THROW(col_int.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_int.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_int.Size(), 0);
    EXPECT_THROW(col_int.ToString(), TypeException);
    EXPECT_THROW(col_int.GetValue(0), TypeException);
    EXPECT_EQ(col_int.tail_index_, 0);
    EXPECT_EQ(col_int.data_type_size_, 4);
    EXPECT_NE(col_int.data_ptr_, nullptr);
    EXPECT_EQ(col_int.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_int.data_type(), data_type);
    EXPECT_EQ(col_int.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_int.buffer_, nullptr);
    EXPECT_NE(col_int.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_int.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorIntegerTest, flat_bigint) {
    using namespace infinity;

    DataType data_type(LogicalType::kBigInt);
    ColumnVector col_bigint(data_type, ColumnVectorType::kFlat);
    col_bigint.Initialize();

    EXPECT_THROW(col_bigint.SetDataType(DataType(LogicalType::kBigInt)), TypeException);
    EXPECT_THROW(col_bigint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_bigint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bigint.Size(), 0);
    EXPECT_THROW(col_bigint.ToString(), TypeException);
    EXPECT_THROW(col_bigint.GetValue(0), TypeException);
    EXPECT_EQ(col_bigint.tail_index_, 0);
    EXPECT_EQ(col_bigint.data_type_size_, 8);
    EXPECT_NE(col_bigint.data_ptr_, nullptr);
    EXPECT_EQ(col_bigint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_bigint.data_type(), data_type);
    EXPECT_EQ(col_bigint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_bigint.buffer_, nullptr);
    EXPECT_NE(col_bigint.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_bigint.GetValue(i + 1), TypeException);
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
        EXPECT_THROW(col_bigint.GetValue(i + 1), TypeException);
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
//    EXPECT_THROW(col_bigint.Initialize(), TypeException);
//    col_bigint.SetDataType(DataType(LogicalType::kBigInt));
//    EXPECT_THROW(col_bigint.Initialize(), TypeException);
//    col_bigint.SetVectorType(ColumnVectorType::kFlat);
    col_bigint.Initialize();
    EXPECT_THROW(col_bigint.SetDataType(DataType(LogicalType::kBigInt)), TypeException);
    EXPECT_THROW(col_bigint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_bigint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bigint.Size(), 0);
    EXPECT_THROW(col_bigint.ToString(), TypeException);
    EXPECT_THROW(col_bigint.GetValue(0), TypeException);
    EXPECT_EQ(col_bigint.tail_index_, 0);
    EXPECT_EQ(col_bigint.data_type_size_, 8);
    EXPECT_NE(col_bigint.data_ptr_, nullptr);
    EXPECT_EQ(col_bigint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_bigint.data_type(), data_type);
    EXPECT_EQ(col_bigint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_bigint.buffer_, nullptr);
    EXPECT_NE(col_bigint.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_bigint.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorIntegerTest, flat_hugeint) {
    using namespace infinity;

    DataType data_type(LogicalType::kHugeInt);
    ColumnVector col_hugeint(data_type, ColumnVectorType::kFlat);
    col_hugeint.Initialize();

    EXPECT_THROW(col_hugeint.SetDataType(DataType(LogicalType::kHugeInt)), TypeException);
    EXPECT_THROW(col_hugeint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_hugeint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_hugeint.Size(), 0);
    EXPECT_THROW(col_hugeint.ToString(), TypeException);
    EXPECT_THROW(col_hugeint.GetValue(0), TypeException);
    EXPECT_EQ(col_hugeint.tail_index_, 0);
    EXPECT_EQ(col_hugeint.data_type_size_, 16);
    EXPECT_NE(col_hugeint.data_ptr_, nullptr);
    EXPECT_EQ(col_hugeint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_hugeint.data_type(), data_type);
    EXPECT_EQ(col_hugeint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_hugeint.buffer_, nullptr);
    EXPECT_NE(col_hugeint.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_hugeint.GetValue(i + 1), TypeException);
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
        EXPECT_THROW(col_hugeint.GetValue(i + 1), TypeException);
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
//    EXPECT_THROW(col_hugeint.Initialize(), TypeException);
//    col_hugeint.SetDataType(DataType(LogicalType::kHugeInt));
//    EXPECT_THROW(col_hugeint.Initialize(), TypeException);
//    col_hugeint.SetVectorType(ColumnVectorType::kFlat);
    col_hugeint.Initialize();
    EXPECT_THROW(col_hugeint.SetDataType(DataType(LogicalType::kHugeInt)), TypeException);
    EXPECT_THROW(col_hugeint.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_hugeint.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_hugeint.Size(), 0);
    EXPECT_THROW(col_hugeint.ToString(), TypeException);
    EXPECT_THROW(col_hugeint.GetValue(0), TypeException);
    EXPECT_EQ(col_hugeint.tail_index_, 0);
    EXPECT_EQ(col_hugeint.data_type_size_, 16);
    EXPECT_NE(col_hugeint.data_ptr_, nullptr);
    EXPECT_EQ(col_hugeint.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_hugeint.data_type(), data_type);
    EXPECT_EQ(col_hugeint.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_hugeint.buffer_, nullptr);
    EXPECT_NE(col_hugeint.nulls_ptr_, nullptr);
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
        EXPECT_THROW(col_hugeint.GetValue(i + 1), TypeException);
    }
}
