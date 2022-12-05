//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ColumnVectorGeoTest : public BaseTest {
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

TEST_F(ColumnVectorGeoTest, flat_point) {
    using namespace infinity;

    DataType data_type(LogicalType::kPoint);
    ColumnVector col_point(data_type, ColumnVectorType::kFlat);
    col_point.Initialize();

    EXPECT_THROW(col_point.SetDataType(DataType(LogicalType::kPoint)), std::logic_error);
    EXPECT_THROW(col_point.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_point.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_point.Size(), 0);
    EXPECT_THROW(col_point.ToString(), std::logic_error);
    EXPECT_THROW(col_point.GetValue(0), std::logic_error);
    EXPECT_EQ(col_point.tail_index_, 0);
    EXPECT_EQ(col_point.data_type_size_, 16);
    EXPECT_NE(col_point.data_ptr_, nullptr);
    EXPECT_EQ(col_point.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_point.data_type(), data_type);
    EXPECT_EQ(col_point.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_point.buffer_, nullptr);
    EXPECT_EQ(col_point.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_point.initialized);
    col_point.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_point.data_ptr_;
    EXPECT_EQ(col_point.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_point.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        col_point.AppendValue(v);
        Value vx = col_point.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        EXPECT_THROW(col_point.GetValue(i + 1), std::logic_error);
    }
    col_point.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_point.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
    }
    EXPECT_EQ(col_point.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_point.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        col_point.AppendValue(v);
        Value vx = col_point.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        EXPECT_THROW(col_point.GetValue(i + 1), std::logic_error);
    }

    col_point.Reset();
    EXPECT_EQ(col_point.capacity(), 0);
    EXPECT_EQ(col_point.tail_index_, 0);
//    EXPECT_EQ(col_point.data_type_size_, 0);
    EXPECT_EQ(col_point.buffer_, nullptr);
    EXPECT_EQ(col_point.data_ptr_, nullptr);
    EXPECT_EQ(col_point.initialized, false);

    // ====
    col_point.Initialize();
    EXPECT_THROW(col_point.SetDataType(DataType(LogicalType::kPoint)), std::logic_error);
    EXPECT_THROW(col_point.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_point.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_point.Size(), 0);
    EXPECT_THROW(col_point.ToString(), std::logic_error);
    EXPECT_THROW(col_point.GetValue(0), std::logic_error);
    EXPECT_EQ(col_point.tail_index_, 0);
    EXPECT_EQ(col_point.data_type_size_, 16);
    EXPECT_NE(col_point.data_ptr_, nullptr);
    EXPECT_EQ(col_point.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_point.data_type(), data_type);
    EXPECT_EQ(col_point.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_point.buffer_, nullptr);
    EXPECT_EQ(col_point.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_point.initialized);
    col_point.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_point.data_ptr_;
    EXPECT_EQ(col_point.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_point.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT point(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        Value v = Value::MakePoint(point);
        col_point.AppendValue(v);
        Value vx = col_point.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPoint);
        EXPECT_FLOAT_EQ(vx.value_.point.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.point.y, static_cast<f64>(i) - 0.8f);
        EXPECT_THROW(col_point.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorGeoTest, flat_line) {
    using namespace infinity;

    DataType data_type(LogicalType::kLine);
    ColumnVector col_line(data_type, ColumnVectorType::kFlat);
    col_line.Initialize();

    EXPECT_THROW(col_line.SetDataType(DataType(LogicalType::kLine)), std::logic_error);
    EXPECT_THROW(col_line.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_line.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_line.Size(), 0);
    EXPECT_THROW(col_line.ToString(), std::logic_error);
    EXPECT_THROW(col_line.GetValue(0), std::logic_error);
    EXPECT_EQ(col_line.tail_index_, 0);
    EXPECT_EQ(col_line.data_type_size_, 24);
    EXPECT_NE(col_line.data_ptr_, nullptr);
    EXPECT_EQ(col_line.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_line.data_type(), data_type);
    EXPECT_EQ(col_line.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_line.buffer_, nullptr);
    EXPECT_EQ(col_line.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_line.initialized);
    col_line.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_line.data_ptr_;
    EXPECT_EQ(col_line.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_line.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        LineT line(static_cast<f64>(i) + 0.5f,
                   static_cast<f64>(i) - 0.8f,
                   static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        col_line.AppendValue(v);
        Value vx = col_line.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        EXPECT_THROW(col_line.GetValue(i + 1), std::logic_error);
    }
    col_line.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_line.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
    }
    EXPECT_EQ(col_line.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_line.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        LineT line(static_cast<f64>(i) + 0.5f,
                   static_cast<f64>(i) - 0.8f,
                   static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        col_line.AppendValue(v);
        Value vx = col_line.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        EXPECT_THROW(col_line.GetValue(i + 1), std::logic_error);
    }

    col_line.Reset();
    EXPECT_EQ(col_line.capacity(), 0);
    EXPECT_EQ(col_line.tail_index_, 0);
//    EXPECT_EQ(col_line.data_type_size_, 0);
    EXPECT_EQ(col_line.buffer_, nullptr);
    EXPECT_EQ(col_line.data_ptr_, nullptr);
    EXPECT_EQ(col_line.initialized, false);

    // ====
    col_line.Initialize();
    EXPECT_THROW(col_line.SetDataType(DataType(LogicalType::kLine)), std::logic_error);
    EXPECT_THROW(col_line.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_line.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_line.Size(), 0);
    EXPECT_THROW(col_line.ToString(), std::logic_error);
    EXPECT_THROW(col_line.GetValue(0), std::logic_error);
    EXPECT_EQ(col_line.tail_index_, 0);
    EXPECT_EQ(col_line.data_type_size_, 24);
    EXPECT_NE(col_line.data_ptr_, nullptr);
    EXPECT_EQ(col_line.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_line.data_type(), data_type);
    EXPECT_EQ(col_line.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_line.buffer_, nullptr);
    EXPECT_EQ(col_line.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_line.initialized);
    col_line.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_line.data_ptr_;
    EXPECT_EQ(col_line.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_line.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        LineT line(static_cast<f64>(i) + 0.5f,
                   static_cast<f64>(i) - 0.8f,
                   static_cast<f64>(i) - 5.3f);
        Value v = Value::MakeLine(line);
        col_line.AppendValue(v);
        Value vx = col_line.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i) - 5.3f);
        EXPECT_THROW(col_line.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorGeoTest, flat_line_seg) {
    using namespace infinity;

    DataType data_type(LogicalType::kLineSeg);
    ColumnVector col_line_seg(data_type, ColumnVectorType::kFlat);
    col_line_seg.Initialize();

    EXPECT_THROW(col_line_seg.SetDataType(DataType(LogicalType::kLineSeg)), std::logic_error);
    EXPECT_THROW(col_line_seg.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_line_seg.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_line_seg.Size(), 0);
    EXPECT_THROW(col_line_seg.ToString(), std::logic_error);
    EXPECT_THROW(col_line_seg.GetValue(0), std::logic_error);
    EXPECT_EQ(col_line_seg.tail_index_, 0);
    EXPECT_EQ(col_line_seg.data_type_size_, 32);
    EXPECT_NE(col_line_seg.data_ptr_, nullptr);
    EXPECT_EQ(col_line_seg.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_line_seg.data_type(), data_type);
    EXPECT_EQ(col_line_seg.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_line_seg.buffer_, nullptr);
    EXPECT_EQ(col_line_seg.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_line_seg.initialized);
    col_line_seg.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_line_seg.data_ptr_;
    EXPECT_EQ(col_line_seg.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_line_seg.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        col_line_seg.AppendValue(v);
        Value vx = col_line_seg.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_line_seg.GetValue(i + 1), std::logic_error);
    }
    col_line_seg.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_line_seg.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
    }
    EXPECT_EQ(col_line_seg.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_line_seg.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        col_line_seg.AppendValue(v);
        Value vx = col_line_seg.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_line_seg.GetValue(i + 1), std::logic_error);
    }

    col_line_seg.Reset();
    EXPECT_EQ(col_line_seg.capacity(), 0);
    EXPECT_EQ(col_line_seg.tail_index_, 0);
//    EXPECT_EQ(col_line_seg.data_type_size_, 0);
    EXPECT_EQ(col_line_seg.buffer_, nullptr);
    EXPECT_EQ(col_line_seg.data_ptr_, nullptr);
    EXPECT_EQ(col_line_seg.initialized, false);

    // ====
    col_line_seg.Initialize();
    EXPECT_THROW(col_line_seg.SetDataType(DataType(LogicalType::kLineSeg)), std::logic_error);
    EXPECT_THROW(col_line_seg.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_line_seg.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_line_seg.Size(), 0);
    EXPECT_THROW(col_line_seg.ToString(), std::logic_error);
    EXPECT_THROW(col_line_seg.GetValue(0), std::logic_error);
    EXPECT_EQ(col_line_seg.tail_index_, 0);
    EXPECT_EQ(col_line_seg.data_type_size_, 32);
    EXPECT_NE(col_line_seg.data_ptr_, nullptr);
    EXPECT_EQ(col_line_seg.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_line_seg.data_type(), data_type);
    EXPECT_EQ(col_line_seg.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_line_seg.buffer_, nullptr);
    EXPECT_EQ(col_line_seg.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_line_seg.initialized);
    col_line_seg.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_line_seg.data_ptr_;
    EXPECT_EQ(col_line_seg.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_line_seg.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        LineSegT line_seg(p1, p2);
        Value v = Value::MakeLineSegment(line_seg);
        col_line_seg.AppendValue(v);
        Value vx = col_line_seg.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLineSeg);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point1.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.line_segment.point2.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_line_seg.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorGeoTest, flat_box) {
    using namespace infinity;

    DataType data_type(LogicalType::kBox);
    ColumnVector col_box(data_type, ColumnVectorType::kFlat);
    col_box.Initialize();

    EXPECT_THROW(col_box.SetDataType(DataType(LogicalType::kBox)), std::logic_error);
    EXPECT_THROW(col_box.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_box.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_box.Size(), 0);
    EXPECT_THROW(col_box.ToString(), std::logic_error);
    EXPECT_THROW(col_box.GetValue(0), std::logic_error);
    EXPECT_EQ(col_box.tail_index_, 0);
    EXPECT_EQ(col_box.data_type_size_, 32);
    EXPECT_NE(col_box.data_ptr_, nullptr);
    EXPECT_EQ(col_box.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_box.data_type(), data_type);
    EXPECT_EQ(col_box.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_box.buffer_, nullptr);
    EXPECT_EQ(col_box.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_box.initialized);
    col_box.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_box.data_ptr_;
    EXPECT_EQ(col_box.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_box.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        col_box.AppendValue(v);
        Value vx = col_box.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_box.GetValue(i + 1), std::logic_error);
    }
    col_box.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_box.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
    }
    EXPECT_EQ(col_box.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_box.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        col_box.AppendValue(v);
        Value vx = col_box.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_box.GetValue(i + 1), std::logic_error);
    }

    col_box.Reset();
    EXPECT_EQ(col_box.capacity(), 0);
    EXPECT_EQ(col_box.tail_index_, 0);
//    EXPECT_EQ(col_box.data_type_size_, 0);
    EXPECT_EQ(col_box.buffer_, nullptr);
    EXPECT_EQ(col_box.data_ptr_, nullptr);
    EXPECT_EQ(col_box.initialized, false);

    // ====
    col_box.Initialize();
    EXPECT_THROW(col_box.SetDataType(DataType(LogicalType::kBox)), std::logic_error);
    EXPECT_THROW(col_box.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_box.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_box.Size(), 0);
    EXPECT_THROW(col_box.ToString(), std::logic_error);
    EXPECT_THROW(col_box.GetValue(0), std::logic_error);
    EXPECT_EQ(col_box.tail_index_, 0);
    EXPECT_EQ(col_box.data_type_size_, 32);
    EXPECT_NE(col_box.data_ptr_, nullptr);
    EXPECT_EQ(col_box.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_box.data_type(), data_type);
    EXPECT_EQ(col_box.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_box.buffer_, nullptr);
    EXPECT_EQ(col_box.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_box.initialized);
    col_box.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_box.data_ptr_;
    EXPECT_EQ(col_box.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_box.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        PointT p2(static_cast<f64>(i) - 5.3f, static_cast<f64>(i) + 7.9f);
        BoxT box(p1, p2);
        Value v = Value::MakeBox(box);
        col_box.AppendValue(v);
        Value vx = col_box.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBox);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.box.upper_left.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.x, static_cast<f64>(i) - 5.3f);
        EXPECT_FLOAT_EQ(vx.value_.box.lower_right.y, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_box.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorGeoTest, flat_path) {

    using namespace infinity;

    DataType data_type(LogicalType::kPath);
    ColumnVector col_path(data_type, ColumnVectorType::kFlat);
    col_path.Initialize();

    EXPECT_THROW(col_path.SetDataType(DataType(LogicalType::kPath)), std::logic_error);
    EXPECT_THROW(col_path.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_path.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_path.Size(), 0);
    EXPECT_THROW(col_path.ToString(), std::logic_error);
    EXPECT_THROW(col_path.GetValue(0), std::logic_error);
    EXPECT_EQ(col_path.tail_index_, 0);
    EXPECT_EQ(col_path.data_type_size_, 16);
    EXPECT_NE(col_path.data_ptr_, nullptr);
    EXPECT_EQ(col_path.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_path.data_type(), data_type);
    EXPECT_EQ(col_path.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_path.buffer_, nullptr);
    EXPECT_EQ(col_path.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_path.initialized);
    col_path.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_path.data_ptr_;
    EXPECT_EQ(col_path.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_path.data_ptr_);


    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        col_path.AppendValue(v);
        Value vx = col_path.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(col_path.GetValue(i + 1), std::logic_error);
    }

    col_path.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = col_path.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 3), p4);
    }

    EXPECT_EQ(col_path.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_path.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        col_path.AppendValue(v);
        Value vx = col_path.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(col_path.GetValue(i + 1), std::logic_error);
    }

    col_path.Reset();
    EXPECT_EQ(col_path.capacity(), 0);
    EXPECT_EQ(col_path.tail_index_, 0);
//    EXPECT_EQ(col_path.data_type_size_, 0);
    EXPECT_EQ(col_path.buffer_, nullptr);
    EXPECT_EQ(col_path.data_ptr_, nullptr);
    EXPECT_EQ(col_path.initialized, false);

    // ====
    col_path.Initialize();
    EXPECT_THROW(col_path.SetDataType(DataType(LogicalType::kPath)), std::logic_error);
    EXPECT_THROW(col_path.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_path.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_path.Size(), 0);
    EXPECT_THROW(col_path.ToString(), std::logic_error);
    EXPECT_THROW(col_path.GetValue(0), std::logic_error);
    EXPECT_EQ(col_path.tail_index_, 0);
    EXPECT_EQ(col_path.data_type_size_, 16);
    EXPECT_NE(col_path.data_ptr_, nullptr);
    EXPECT_EQ(col_path.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_path.data_type(), data_type);
    EXPECT_EQ(col_path.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_path.buffer_, nullptr);
    EXPECT_EQ(col_path.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_path.initialized);
    col_path.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_path.data_ptr_;
    EXPECT_EQ(col_path.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_path.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PathT path;
        path.Initialize(4, 0);
        path.SetPoint(0, p1);
        path.SetPoint(1, p2);
        path.SetPoint(2, p3);
        path.SetPoint(3, p4);
        Value v = Value::MakePath(path);
        col_path.AppendValue(v);
        Value vx = col_path.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kPath);
        EXPECT_EQ(vx.value_.path.point_count, 4);
        EXPECT_EQ(vx.value_.path.closed, 0);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.path.ptr) + 3), p4);

        EXPECT_THROW(col_path.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorGeoTest, flat_polygon) {

    using namespace infinity;

    DataType data_type(LogicalType::kPolygon);
    ColumnVector col_polygon(data_type, ColumnVectorType::kFlat);
    col_polygon.Initialize();

    EXPECT_THROW(col_polygon.SetDataType(DataType(LogicalType::kPolygon)), std::logic_error);
    EXPECT_THROW(col_polygon.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_polygon.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_polygon.Size(), 0);
    EXPECT_THROW(col_polygon.ToString(), std::logic_error);
    EXPECT_THROW(col_polygon.GetValue(0), std::logic_error);
    EXPECT_EQ(col_polygon.tail_index_, 0);
    EXPECT_EQ(col_polygon.data_type_size_, 48);
    EXPECT_NE(col_polygon.data_ptr_, nullptr);
    EXPECT_EQ(col_polygon.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_polygon.data_type(), data_type);
    EXPECT_EQ(col_polygon.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_polygon.buffer_, nullptr);
    EXPECT_EQ(col_polygon.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_polygon.initialized);
    col_polygon.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_polygon.data_ptr_;
    EXPECT_EQ(col_polygon.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_polygon.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PolygonT polygon;
        polygon.Initialize(4);
        polygon.SetPoint(0, p1);
        polygon.SetPoint(1, p2);
        polygon.SetPoint(2, p3);
        polygon.SetPoint(3, p4);
        Value v = Value::MakePolygon(polygon);
        col_polygon.AppendValue(v);
        Value vx = col_polygon.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
        EXPECT_EQ(vx.type().type(), LogicalType::kPolygon);
        EXPECT_EQ(vx.value_.polygon.point_count, 4);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 3), p4);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.x, bounding_box.upper_left.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.y, bounding_box.upper_left.y);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.x, bounding_box.lower_right.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.y, bounding_box.lower_right.y);

        EXPECT_THROW(col_polygon.GetValue(i + 1), std::logic_error);
    }


    col_polygon.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);

        Value vx = col_polygon.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
        EXPECT_EQ(vx.type().type(), LogicalType::kPolygon);
        EXPECT_EQ(vx.value_.polygon.point_count, 4);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 3), p4);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.x, bounding_box.upper_left.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.y, bounding_box.upper_left.y);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.x, bounding_box.lower_right.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.y, bounding_box.lower_right.y);
    }

    EXPECT_EQ(col_polygon.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_polygon.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PolygonT polygon;
        polygon.Initialize(4);
        polygon.SetPoint(0, p1);
        polygon.SetPoint(1, p2);
        polygon.SetPoint(2, p3);
        polygon.SetPoint(3, p4);
        Value v = Value::MakePolygon(polygon);
        col_polygon.AppendValue(v);
        Value vx = col_polygon.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
        EXPECT_EQ(vx.type().type(), LogicalType::kPolygon);
        EXPECT_EQ(vx.value_.polygon.point_count, 4);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 3), p4);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.x, bounding_box.upper_left.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.y, bounding_box.upper_left.y);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.x, bounding_box.lower_right.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.y, bounding_box.lower_right.y);

        EXPECT_THROW(col_polygon.GetValue(i + 1), std::logic_error);
    }

    col_polygon.Reset();
    EXPECT_EQ(col_polygon.capacity(), 0);
    EXPECT_EQ(col_polygon.tail_index_, 0);
//    EXPECT_EQ(col_polygon.data_type_size_, 0);
    EXPECT_EQ(col_polygon.buffer_, nullptr);
    EXPECT_EQ(col_polygon.data_ptr_, nullptr);
    EXPECT_EQ(col_polygon.initialized, false);

    // ====
    col_polygon.Initialize();
    EXPECT_THROW(col_polygon.SetDataType(DataType(LogicalType::kPolygon)), std::logic_error);
    EXPECT_THROW(col_polygon.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_polygon.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_polygon.Size(), 0);
    EXPECT_THROW(col_polygon.ToString(), std::logic_error);
    EXPECT_THROW(col_polygon.GetValue(0), std::logic_error);
    EXPECT_EQ(col_polygon.tail_index_, 0);
    EXPECT_EQ(col_polygon.data_type_size_, 48);
    EXPECT_NE(col_polygon.data_ptr_, nullptr);
    EXPECT_EQ(col_polygon.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_polygon.data_type(), data_type);
    EXPECT_EQ(col_polygon.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_polygon.buffer_, nullptr);
    EXPECT_EQ(col_polygon.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_polygon.initialized);
    col_polygon.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_polygon.data_ptr_;
    EXPECT_EQ(col_polygon.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_polygon.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f);
        PointT p2(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.7f);
        PointT p3(static_cast<f64>(i) + 0.2f, static_cast<f64>(i) - 0.4f);
        PointT p4(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f);
        PolygonT polygon;
        polygon.Initialize(4);
        polygon.SetPoint(0, p1);
        polygon.SetPoint(1, p2);
        polygon.SetPoint(2, p3);
        polygon.SetPoint(3, p4);
        Value v = Value::MakePolygon(polygon);
        col_polygon.AppendValue(v);
        Value vx = col_polygon.GetValue(i);
        BoxT bounding_box(PointT(static_cast<f64>(i) + 0.1f, static_cast<f64>(i) - 0.3f),
                          PointT(static_cast<f64>(i) + 0.6f, static_cast<f64>(i) - 0.8f));
        EXPECT_EQ(vx.type().type(), LogicalType::kPolygon);
        EXPECT_EQ(vx.value_.polygon.point_count, 4);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr)), p1);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 1), p2);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 2), p3);
        EXPECT_EQ(*((PointT*)(vx.value_.polygon.ptr) + 3), p4);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.x, bounding_box.upper_left.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.upper_left.y, bounding_box.upper_left.y);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.x, bounding_box.lower_right.x);
        EXPECT_DOUBLE_EQ(vx.value_.polygon.bounding_box.lower_right.y, bounding_box.lower_right.y);

        EXPECT_THROW(col_polygon.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorGeoTest, flat_circle) {
    using namespace infinity;

    DataType data_type(LogicalType::kCircle);
    ColumnVector col_circle(data_type, ColumnVectorType::kFlat);
    col_circle.Initialize();

    EXPECT_THROW(col_circle.SetDataType(DataType(LogicalType::kCircle)), std::logic_error);
    EXPECT_THROW(col_circle.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_circle.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_circle.Size(), 0);
    EXPECT_THROW(col_circle.ToString(), std::logic_error);
    EXPECT_THROW(col_circle.GetValue(0), std::logic_error);
    EXPECT_EQ(col_circle.tail_index_, 0);
    EXPECT_EQ(col_circle.data_type_size_, 24);
    EXPECT_NE(col_circle.data_ptr_, nullptr);
    EXPECT_EQ(col_circle.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_circle.data_type(), data_type);
    EXPECT_EQ(col_circle.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_circle.buffer_, nullptr);
    EXPECT_EQ(col_circle.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_circle.initialized);
    col_circle.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_circle.data_ptr_;
    EXPECT_EQ(col_circle.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_circle.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        col_circle.AppendValue(v);
        Value vx = col_circle.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_circle.GetValue(i + 1), std::logic_error);
    }
    col_circle.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_circle.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
    }
    EXPECT_EQ(col_circle.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_circle.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        col_circle.AppendValue(v);
        Value vx = col_circle.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_circle.GetValue(i + 1), std::logic_error);
    }

    col_circle.Reset();
    EXPECT_EQ(col_circle.capacity(), 0);
    EXPECT_EQ(col_circle.tail_index_, 0);
//    EXPECT_EQ(col_circle.data_type_size_, 0);
    EXPECT_EQ(col_circle.buffer_, nullptr);
    EXPECT_EQ(col_circle.data_ptr_, nullptr);
    EXPECT_EQ(col_circle.initialized, false);

    // ====
    col_circle.Initialize();
    EXPECT_THROW(col_circle.SetDataType(DataType(LogicalType::kCircle)), std::logic_error);
    EXPECT_THROW(col_circle.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_circle.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_circle.Size(), 0);
    EXPECT_THROW(col_circle.ToString(), std::logic_error);
    EXPECT_THROW(col_circle.GetValue(0), std::logic_error);
    EXPECT_EQ(col_circle.tail_index_, 0);
    EXPECT_EQ(col_circle.data_type_size_, 24);
    EXPECT_NE(col_circle.data_ptr_, nullptr);
    EXPECT_EQ(col_circle.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_circle.data_type(), data_type);
    EXPECT_EQ(col_circle.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_circle.buffer_, nullptr);
    EXPECT_EQ(col_circle.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_circle.initialized);
    col_circle.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_circle.data_ptr_;
    EXPECT_EQ(col_circle.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_circle.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i) + 0.5f, static_cast<f64>(i) - 0.8f);
        f64 r = static_cast<f64>(i) + 7.9f;
        CircleT circle(p1, r);
        Value v = Value::MakeCircle(circle);
        col_circle.AppendValue(v);
        Value vx = col_circle.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i) + 0.5f);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i) - 0.8f);
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i) + 7.9f);
        EXPECT_THROW(col_circle.GetValue(i + 1), std::logic_error);
    }
}


