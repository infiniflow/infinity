//
// Created by JinHai on 2022/12/5.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ColumnVectorBitmapTest : public BaseTest {
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

TEST_F(ColumnVectorBitmapTest, flat_bitmap) {

    using namespace infinity;

    DataType data_type(LogicalType::kBitmap);
    ColumnVector col_bitmap(data_type, ColumnVectorType::kFlat);

    col_bitmap.Initialize();

    EXPECT_THROW(col_bitmap.SetDataType(DataType(LogicalType::kBitmap)), std::logic_error);
    EXPECT_THROW(col_bitmap.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_bitmap.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bitmap.Size(), 0);
    EXPECT_THROW(col_bitmap.ToString(), std::logic_error);
    EXPECT_THROW(col_bitmap.GetValue(0), std::logic_error);
    EXPECT_EQ(col_bitmap.tail_index_, 0);
    EXPECT_EQ(col_bitmap.data_type_size_, 16);
    EXPECT_NE(col_bitmap.data_ptr_, nullptr);
    EXPECT_EQ(col_bitmap.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_bitmap.data_type(), data_type);
    EXPECT_EQ(col_bitmap.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_bitmap.buffer_, nullptr);
    EXPECT_EQ(col_bitmap.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_bitmap.initialized);
    col_bitmap.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_bitmap.data_ptr_;
    EXPECT_EQ(col_bitmap.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_bitmap.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value v = Value::MakeBitmap(bitmap);
        col_bitmap.AppendValue(v);
        Value vx = col_bitmap.GetValue(i);

        EXPECT_EQ(vx.value_.bitmap, bitmap);
        EXPECT_THROW(col_bitmap.GetValue(i + 1), std::logic_error);
    }


    col_bitmap.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value vx = col_bitmap.GetValue(i);
        EXPECT_EQ(vx.value_.bitmap, bitmap);
    }

    EXPECT_EQ(col_bitmap.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bitmap.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value v = Value::MakeBitmap(bitmap);
        col_bitmap.AppendValue(v);
        Value vx = col_bitmap.GetValue(i);

        EXPECT_EQ(vx.value_.bitmap, bitmap);
        EXPECT_THROW(col_bitmap.GetValue(i + 1), std::logic_error);
    }

    col_bitmap.Reset();
    EXPECT_EQ(col_bitmap.capacity(), 0);
    EXPECT_EQ(col_bitmap.tail_index_, 0);
//    EXPECT_EQ(col_bitmap.data_type_size_, 0);
    EXPECT_EQ(col_bitmap.buffer_, nullptr);
    EXPECT_EQ(col_bitmap.data_ptr_, nullptr);
    EXPECT_EQ(col_bitmap.initialized, false);

    // ====
    col_bitmap.Initialize();
    EXPECT_THROW(col_bitmap.SetDataType(DataType(LogicalType::kBitmap)), std::logic_error);
    EXPECT_THROW(col_bitmap.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_bitmap.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_bitmap.Size(), 0);
    EXPECT_THROW(col_bitmap.ToString(), std::logic_error);
    EXPECT_THROW(col_bitmap.GetValue(0), std::logic_error);
    EXPECT_EQ(col_bitmap.tail_index_, 0);
    EXPECT_EQ(col_bitmap.data_type_size_, 16);
    EXPECT_NE(col_bitmap.data_ptr_, nullptr);
    EXPECT_EQ(col_bitmap.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_bitmap.data_type(), data_type);
    EXPECT_EQ(col_bitmap.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_bitmap.buffer_, nullptr);
    EXPECT_EQ(col_bitmap.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_bitmap.initialized);
    col_bitmap.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_bitmap.data_ptr_;
    EXPECT_EQ(col_bitmap.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_bitmap.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value v = Value::MakeBitmap(bitmap);
        col_bitmap.AppendValue(v);
        Value vx = col_bitmap.GetValue(i);

        EXPECT_EQ(vx.value_.bitmap, bitmap);
        EXPECT_THROW(col_bitmap.GetValue(i + 1), std::logic_error);
    }
}