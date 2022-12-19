//
// Created by JinHai on 2022/12/5.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"

class ColumnVectorBlobTest : public BaseTest {
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

TEST_F(ColumnVectorBlobTest, flat_blob) {
    using namespace infinity;

    DataType data_type(LogicalType::kBlob);
    ColumnVector col_blob(data_type, ColumnVectorType::kFlat);
    col_blob.Initialize();

    EXPECT_THROW(col_blob.SetDataType(DataType(LogicalType::kBlob)), TypeException);
    EXPECT_THROW(col_blob.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_blob.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_blob.Size(), 0);
    EXPECT_THROW(col_blob.ToString(), TypeException);
    EXPECT_THROW(col_blob.GetValue(0), TypeException);
    EXPECT_EQ(col_blob.tail_index_, 0);
    EXPECT_EQ(col_blob.data_type_size_, 16);
    EXPECT_NE(col_blob.data_ptr_, nullptr);
    EXPECT_EQ(col_blob.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_blob.data_type(), data_type);
    EXPECT_EQ(col_blob.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_blob.buffer_, nullptr);
    EXPECT_NE(col_blob.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_blob.initialized);
    col_blob.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_blob.data_ptr_;
    EXPECT_EQ(col_blob.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_blob.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        GlobalResourceUsage::IncrRawMemCount();

        for(i64 j = 0; j < blob_len; ++ j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        col_blob.AppendValue(v);
        Value vx = col_blob.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(col_blob.GetValue(i + 1), TypeException);
    }

    col_blob.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        GlobalResourceUsage::IncrRawMemCount();

        for(i64 j = 0; j < blob_len; ++ j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);
        Value vx = col_blob.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
    }

    EXPECT_EQ(col_blob.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_blob.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        GlobalResourceUsage::IncrRawMemCount();

        for(i64 j = 0; j < blob_len; ++ j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        col_blob.AppendValue(v);
        Value vx = col_blob.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(col_blob.GetValue(i + 1), TypeException);
    }

    col_blob.Reset();
    EXPECT_EQ(col_blob.capacity(), 0);
    EXPECT_EQ(col_blob.tail_index_, 0);
    EXPECT_EQ(col_blob.buffer_, nullptr);
    EXPECT_EQ(col_blob.data_ptr_, nullptr);
    EXPECT_EQ(col_blob.initialized, false);

    // ====
    col_blob.Initialize();
    EXPECT_THROW(col_blob.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_blob.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_blob.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_blob.Size(), 0);
    EXPECT_THROW(col_blob.ToString(), TypeException);
    EXPECT_THROW(col_blob.GetValue(0), TypeException);
    EXPECT_EQ(col_blob.tail_index_, 0);
    EXPECT_EQ(col_blob.data_type_size_, 16);
    EXPECT_NE(col_blob.data_ptr_, nullptr);
    EXPECT_EQ(col_blob.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_blob.data_type(), data_type);
    EXPECT_EQ(col_blob.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_blob.buffer_, nullptr);
    EXPECT_NE(col_blob.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_blob.initialized);
    col_blob.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_blob.data_ptr_;
    EXPECT_EQ(col_blob.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_blob.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        GlobalResourceUsage::IncrRawMemCount();

        for(i64 j = 0; j < blob_len; ++ j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        col_blob.AppendValue(v);
        Value vx = col_blob.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
        EXPECT_THROW(col_blob.GetValue(i + 1), TypeException);
    }
}