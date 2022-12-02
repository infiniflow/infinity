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
//
//    col_double.Reserve(DEFAULT_VECTOR_SIZE* 2);
//
//        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
//            Value vx = col_double.GetValue(i);
//            EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
//            EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
//        }
//
//    EXPECT_EQ(col_double.tail_index_, DEFAULT_VECTOR_SIZE);
//    EXPECT_EQ(col_double.capacity(), 2* DEFAULT_VECTOR_SIZE);
//    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
//        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
//        col_double.AppendValue(v);
//        Value vx = col_double.GetValue(i);
//        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
//        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
//        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
//    }
//
//    col_double.Reset();
//    EXPECT_EQ(col_double.capacity(), 0);
//    EXPECT_EQ(col_double.tail_index_, 0);
//    EXPECT_EQ(col_double.buffer_, nullptr);
//    EXPECT_EQ(col_double.data_ptr_, nullptr);
//    EXPECT_EQ(col_double.initialized, false);
//
//    // ====
//    col_double.Initialize();
//    EXPECT_THROW(col_double.SetDataType(DataType(LogicalType::kDouble)), std::logic_error);
//    EXPECT_THROW(col_double.SetVectorType(ColumnVectorType::kFlat), std::logic_error);
//
//    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
//    EXPECT_EQ(col_double.Size(), 0);
//    EXPECT_THROW(col_double.ToString(), std::logic_error);
//    EXPECT_THROW(col_double.GetValue(0), std::logic_error);
//    EXPECT_EQ(col_double.tail_index_, 0);
//    EXPECT_EQ(col_double.data_type_size_, 8);
//    EXPECT_NE(col_double.data_ptr_, nullptr);
//    EXPECT_EQ(col_double.vector_type(), ColumnVectorType::kFlat);
//    EXPECT_EQ(col_double.data_type(), data_type);
//
//    EXPECT_NE(col_double.buffer_, nullptr);
//    EXPECT_EQ(col_double.nulls_ptr_, nullptr);
//    EXPECT_TRUE(col_double.initialized);
//    col_double.Reserve(DEFAULT_VECTOR_SIZE - 1);
//    tmp_ptr = col_double.data_ptr_;
//    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
//    EXPECT_EQ(tmp_ptr, col_double.data_ptr_);
//
//    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
//        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
//        col_double.AppendValue(v);
//        Value vx = col_double.GetValue(i);
//        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
//        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
//        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
//    }
}

