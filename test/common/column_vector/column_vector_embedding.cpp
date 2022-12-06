//
// Created by JinHai on 2022/12/6.
//


#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/embedding_info.h"

class ColumnVectorEmbeddingTest : public BaseTest {
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

TEST_F(ColumnVectorEmbeddingTest, flat_embedding) {
    using namespace infinity;

    auto embedding_info = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 16);
    DataType data_type(LogicalType::kEmbedding, embedding_info);
    ColumnVector col_embedding(data_type, ColumnVectorType::kFlat);
    col_embedding.Initialize();

    EXPECT_THROW(col_embedding.SetDataType(DataType(LogicalType::kEmbedding)), std::logic_error);
    EXPECT_THROW(col_embedding.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_embedding.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_embedding.Size(), 0);
    EXPECT_THROW(col_embedding.ToString(), std::logic_error);
    EXPECT_THROW(col_embedding.GetValue(0), std::logic_error);
    EXPECT_EQ(col_embedding.tail_index_, 0);
    EXPECT_EQ(col_embedding.data_type_size_, 4 * 16);
    EXPECT_NE(col_embedding.data_ptr_, nullptr);
    EXPECT_EQ(col_embedding.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_embedding.data_type(), data_type);
    EXPECT_EQ(col_embedding.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_embedding.buffer_, nullptr);
    EXPECT_EQ(col_embedding.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_embedding.initialized);
    col_embedding.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_embedding.data_ptr_;
    EXPECT_EQ(col_embedding.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_embedding.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeEmbedding(embedding_info->Type(), embedding_info->Dimension());
        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            ((float*)(v.value_.embedding.ptr))[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }
        col_embedding.AppendValue(v);
        Value vx = col_embedding.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->Size(), 64);

        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            EXPECT_FLOAT_EQ(((float*)(vx.value_.embedding.ptr))[j], ((float*)(v.value_.embedding.ptr))[j]);
        }

        v.value_.embedding.Reset();
        EXPECT_THROW(col_embedding.GetValue(i + 1), std::logic_error);
    }

    col_embedding.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeEmbedding(embedding_info->Type(), embedding_info->Dimension());
        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            ((float*)(v.value_.embedding.ptr))[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }

        Value vx = col_embedding.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->Size(), 64);

        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            EXPECT_FLOAT_EQ(((float*)(vx.value_.embedding.ptr))[j], ((float*)(v.value_.embedding.ptr))[j]);
        }

        v.value_.embedding.Reset();
    }

    EXPECT_EQ(col_embedding.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_embedding.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeEmbedding(embedding_info->Type(), embedding_info->Dimension());
        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            ((float*)(v.value_.embedding.ptr))[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }
        col_embedding.AppendValue(v);
        Value vx = col_embedding.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->Size(), 64);

        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            EXPECT_FLOAT_EQ(((float*)(vx.value_.embedding.ptr))[j], ((float*)(v.value_.embedding.ptr))[j]);
        }

        v.value_.embedding.Reset();
        EXPECT_THROW(col_embedding.GetValue(i + 1), std::logic_error);
    }

    col_embedding.Reset();
    EXPECT_EQ(col_embedding.capacity(), 0);
    EXPECT_EQ(col_embedding.tail_index_, 0);
    EXPECT_EQ(col_embedding.buffer_, nullptr);
    EXPECT_EQ(col_embedding.data_ptr_, nullptr);
    EXPECT_EQ(col_embedding.initialized, false);

    // ====
    col_embedding.Initialize();
    EXPECT_THROW(col_embedding.SetDataType(DataType(LogicalType::kDecimal128)), std::logic_error);
    EXPECT_THROW(col_embedding.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_embedding.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_embedding.Size(), 0);
    EXPECT_THROW(col_embedding.ToString(), std::logic_error);
    EXPECT_THROW(col_embedding.GetValue(0), std::logic_error);
    EXPECT_EQ(col_embedding.tail_index_, 0);
    EXPECT_EQ(col_embedding.data_type_size_, 16 * 4);
    EXPECT_NE(col_embedding.data_ptr_, nullptr);
    EXPECT_EQ(col_embedding.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_embedding.data_type(), data_type);
    EXPECT_EQ(col_embedding.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_embedding.buffer_, nullptr);
    EXPECT_EQ(col_embedding.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_embedding.initialized);
    col_embedding.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_embedding.data_ptr_;
    EXPECT_EQ(col_embedding.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_embedding.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeEmbedding(embedding_info->Type(), embedding_info->Dimension());
        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            ((float*)(v.value_.embedding.ptr))[j] = static_cast<float>(i) + static_cast<float>(j) + 0.5f;
        }
        col_embedding.AppendValue(v);
        Value vx = col_embedding.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kEmbedding);
        EXPECT_EQ(vx.type().type_info()->Size(), 64);

        for(i64 j = 0; j < embedding_info->Dimension(); ++ j) {
            EXPECT_FLOAT_EQ(((float*)(vx.value_.embedding.ptr))[j], ((float*)(v.value_.embedding.ptr))[j]);
        }

        v.value_.embedding.Reset();
        EXPECT_THROW(col_embedding.GetValue(i + 1), std::logic_error);
    }
}