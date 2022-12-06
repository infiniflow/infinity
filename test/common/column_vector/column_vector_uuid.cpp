//
// Created by JinHai on 2022/12/6.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ColumnVectorUuidTest : public BaseTest {
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

TEST_F(ColumnVectorUuidTest, flat_uuid) {
    using namespace infinity;

    DataType data_type(LogicalType::kUuid);
    ColumnVector col_uuid(data_type, ColumnVectorType::kFlat);
    col_uuid.Initialize();

    EXPECT_THROW(col_uuid.SetDataType(DataType(LogicalType::kUuid)), std::logic_error);
    EXPECT_THROW(col_uuid.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_uuid.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_uuid.Size(), 0);
    EXPECT_THROW(col_uuid.ToString(), std::logic_error);
    EXPECT_THROW(col_uuid.GetValue(0), std::logic_error);
    EXPECT_EQ(col_uuid.tail_index_, 0);
    EXPECT_EQ(col_uuid.data_type_size_, 16);
    EXPECT_NE(col_uuid.data_ptr_, nullptr);
    EXPECT_EQ(col_uuid.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_uuid.data_type(), data_type);
    EXPECT_EQ(col_uuid.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_uuid.buffer_, nullptr);
    EXPECT_EQ(col_uuid.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_uuid.initialized);
    col_uuid.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_uuid.data_ptr_;
    EXPECT_EQ(col_uuid.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_uuid.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        col_uuid.AppendValue(v);
        Value vx = col_uuid.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
        EXPECT_THROW(col_uuid.GetValue(i + 1), std::logic_error);
    }

    col_uuid.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value vx = col_uuid.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
    }

    EXPECT_EQ(col_uuid.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_uuid.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        col_uuid.AppendValue(v);
        Value vx = col_uuid.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
        EXPECT_THROW(col_uuid.GetValue(i + 1), std::logic_error);
    }

    col_uuid.Reset();
    EXPECT_EQ(col_uuid.capacity(), 0);
    EXPECT_EQ(col_uuid.tail_index_, 0);
    EXPECT_EQ(col_uuid.buffer_, nullptr);
    EXPECT_EQ(col_uuid.data_ptr_, nullptr);
    EXPECT_EQ(col_uuid.initialized, false);

    // ====
    col_uuid.Initialize();
    EXPECT_THROW(col_uuid.SetDataType(DataType(LogicalType::kDecimal128)), std::logic_error);
    EXPECT_THROW(col_uuid.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_uuid.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_uuid.Size(), 0);
    EXPECT_THROW(col_uuid.ToString(), std::logic_error);
    EXPECT_THROW(col_uuid.GetValue(0), std::logic_error);
    EXPECT_EQ(col_uuid.tail_index_, 0);
    EXPECT_EQ(col_uuid.data_type_size_, 16);
    EXPECT_NE(col_uuid.data_ptr_, nullptr);
    EXPECT_EQ(col_uuid.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_uuid.data_type(), data_type);
    EXPECT_EQ(col_uuid.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_uuid.buffer_, nullptr);
    EXPECT_EQ(col_uuid.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_uuid.initialized);
    col_uuid.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_uuid.data_ptr_;
    EXPECT_EQ(col_uuid.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_uuid.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        col_uuid.AppendValue(v);
        Value vx = col_uuid.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
        EXPECT_THROW(col_uuid.GetValue(i + 1), std::logic_error);
    }
}
