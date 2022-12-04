//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

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

TEST_F(ColumnVectorTest, flat_date) {
    using namespace infinity;

    DataType data_type(LogicalType::kDate);
    ColumnVector col_date(data_type, ColumnVectorType::kFlat);
    col_date.Initialize();

    EXPECT_THROW(col_date.SetDataType(DataType(LogicalType::kDate)), std::logic_error);
    EXPECT_THROW(col_date.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_date.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_date.Size(), 0);
    EXPECT_THROW(col_date.ToString(), std::logic_error);
    EXPECT_THROW(col_date.GetValue(0), std::logic_error);
    EXPECT_EQ(col_date.tail_index_, 0);
    EXPECT_EQ(col_date.data_type_size_, 4);
    EXPECT_NE(col_date.data_ptr_, nullptr);
    EXPECT_EQ(col_date.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_date.data_type(), data_type);
    EXPECT_EQ(col_date.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_date.buffer_, nullptr);
    EXPECT_EQ(col_date.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_date.initialized);
    col_date.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_date.data_ptr_;
    EXPECT_EQ(col_date.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_date.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        DateT date;
        date.value = static_cast<i32>(i);
        Value v = Value::MakeDate(date);
        col_date.AppendValue(v);
        Value vx = col_date.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDate);
        EXPECT_EQ(vx.value_.date.value, static_cast<i32>(i));
        EXPECT_THROW(col_date.GetValue(i + 1), std::logic_error);
    }

    col_date.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_date.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDate);
        EXPECT_EQ(vx.value_.date.value, static_cast<i32>(i));
    }
    EXPECT_EQ(col_date.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_date.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        DateT date;
        date.value = static_cast<i32>(i);
        Value v = Value::MakeDate(date);
        col_date.AppendValue(v);
        Value vx = col_date.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDate);
        EXPECT_EQ(vx.value_.date.value, static_cast<i32>(i));
        EXPECT_THROW(col_date.GetValue(i + 1), std::logic_error);
    }

    col_date.Reset();
    EXPECT_EQ(col_date.capacity(), 0);
    EXPECT_EQ(col_date.tail_index_, 0);
//    EXPECT_EQ(col_tinyint.data_type_size_, 0);
    EXPECT_EQ(col_date.buffer_, nullptr);
    EXPECT_EQ(col_date.data_ptr_, nullptr);
    EXPECT_EQ(col_date.initialized, false);

    // ====
    col_date.Initialize();
    EXPECT_THROW(col_date.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_date.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_date.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_date.Size(), 0);
    EXPECT_THROW(col_date.ToString(), std::logic_error);
    EXPECT_THROW(col_date.GetValue(0), std::logic_error);
    EXPECT_EQ(col_date.tail_index_, 0);
    EXPECT_EQ(col_date.data_type_size_, 4);
    EXPECT_NE(col_date.data_ptr_, nullptr);
    EXPECT_EQ(col_date.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_date.data_type(), data_type);
    EXPECT_EQ(col_date.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_date.buffer_, nullptr);
    EXPECT_EQ(col_date.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_date.initialized);
    col_date.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_date.data_ptr_;
    EXPECT_EQ(col_date.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_date.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        DateT date;
        date.value = static_cast<i32>(i);
        Value v = Value::MakeDate(date);
        col_date.AppendValue(v);
        Value vx = col_date.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDate);
        EXPECT_EQ(vx.value_.date.value, static_cast<i32>(i));
        EXPECT_THROW(col_date.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_time) {
    using namespace infinity;

    DataType data_type(LogicalType::kTime);
    ColumnVector col_time(data_type, ColumnVectorType::kFlat);
    col_time.Initialize();

    EXPECT_THROW(col_time.SetDataType(DataType(LogicalType::kTime)), std::logic_error);
    EXPECT_THROW(col_time.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_time.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_time.Size(), 0);
    EXPECT_THROW(col_time.ToString(), std::logic_error);
    EXPECT_THROW(col_time.GetValue(0), std::logic_error);
    EXPECT_EQ(col_time.tail_index_, 0);
    EXPECT_EQ(col_time.data_type_size_, 4);
    EXPECT_NE(col_time.data_ptr_, nullptr);
    EXPECT_EQ(col_time.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_time.data_type(), data_type);
    EXPECT_EQ(col_time.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_time.buffer_, nullptr);
    EXPECT_EQ(col_time.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_time.initialized);
    col_time.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_time.data_ptr_;
    EXPECT_EQ(col_time.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_time.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        TimeT time;
        time.value = static_cast<i32>(i);
        Value v = Value::MakeTime(time);
        col_time.AppendValue(v);
        Value vx = col_time.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTime);
        EXPECT_EQ(vx.value_.time.value, static_cast<i32>(i));
        EXPECT_THROW(col_time.GetValue(i + 1), std::logic_error);
    }

    col_time.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_time.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTime);
        EXPECT_EQ(vx.value_.time.value, static_cast<i32>(i));
    }
    EXPECT_EQ(col_time.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_time.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        TimeT time;
        time.value = static_cast<i32>(i);
        Value v = Value::MakeTime(time);
        col_time.AppendValue(v);
        Value vx = col_time.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTime);
        EXPECT_EQ(vx.value_.time.value, static_cast<i32>(i));
        EXPECT_THROW(col_time.GetValue(i + 1), std::logic_error);
    }

    col_time.Reset();
    EXPECT_EQ(col_time.capacity(), 0);
    EXPECT_EQ(col_time.tail_index_, 0);
//    EXPECT_EQ(col_tinyint.data_type_size_, 0);
    EXPECT_EQ(col_time.buffer_, nullptr);
    EXPECT_EQ(col_time.data_ptr_, nullptr);
    EXPECT_EQ(col_time.initialized, false);

    // ====
    col_time.Initialize();
    EXPECT_THROW(col_time.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_time.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_time.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_time.Size(), 0);
    EXPECT_THROW(col_time.ToString(), std::logic_error);
    EXPECT_THROW(col_time.GetValue(0), std::logic_error);
    EXPECT_EQ(col_time.tail_index_, 0);
    EXPECT_EQ(col_time.data_type_size_, 4);
    EXPECT_NE(col_time.data_ptr_, nullptr);
    EXPECT_EQ(col_time.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_time.data_type(), data_type);
    EXPECT_EQ(col_time.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_time.buffer_, nullptr);
    EXPECT_EQ(col_time.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_time.initialized);
    col_time.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_time.data_ptr_;
    EXPECT_EQ(col_time.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_time.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        TimeT time;
        time.value = static_cast<i32>(i);
        Value v = Value::MakeTime(time);
        col_time.AppendValue(v);
        Value vx = col_time.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTime);
        EXPECT_EQ(vx.value_.time.value, static_cast<i32>(i));
        EXPECT_THROW(col_time.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_datetime) {
    using namespace infinity;

    DataType data_type(LogicalType::kDateTime);
    ColumnVector col_datetime(data_type, ColumnVectorType::kFlat);
    col_datetime.Initialize();

    EXPECT_THROW(col_datetime.SetDataType(DataType(LogicalType::kDateTime)), std::logic_error);
    EXPECT_THROW(col_datetime.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_datetime.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_datetime.Size(), 0);
    EXPECT_THROW(col_datetime.ToString(), std::logic_error);
    EXPECT_THROW(col_datetime.GetValue(0), std::logic_error);
    EXPECT_EQ(col_datetime.tail_index_, 0);
    EXPECT_EQ(col_datetime.data_type_size_, 8);
    EXPECT_NE(col_datetime.data_ptr_, nullptr);
    EXPECT_EQ(col_datetime.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_datetime.data_type(), data_type);
    EXPECT_EQ(col_datetime.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_datetime.buffer_, nullptr);
    EXPECT_EQ(col_datetime.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_datetime.initialized);
    col_datetime.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_datetime.data_ptr_;
    EXPECT_EQ(col_datetime.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_datetime.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        DateTimeT datetime;
        datetime.date = static_cast<i32>(i);
        datetime.time = static_cast<i32>(i);
        Value v = Value::MakeDateTime(datetime);
        col_datetime.AppendValue(v);
        Value vx = col_datetime.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDateTime);
        EXPECT_EQ(vx.value_.datetime.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.datetime.time, static_cast<i32>(i));
        EXPECT_THROW(col_datetime.GetValue(i + 1), std::logic_error);
    }

    col_datetime.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_datetime.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDateTime);
        EXPECT_EQ(vx.value_.datetime.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.datetime.time, static_cast<i32>(i));
    }
    EXPECT_EQ(col_datetime.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_datetime.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        DateTimeT datetime;
        datetime.date = static_cast<i32>(i);
        datetime.time = static_cast<i32>(i);
        Value v = Value::MakeDateTime(datetime);
        col_datetime.AppendValue(v);
        Value vx = col_datetime.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDateTime);
        EXPECT_EQ(vx.value_.datetime.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.datetime.time, static_cast<i32>(i));
        EXPECT_THROW(col_datetime.GetValue(i + 1), std::logic_error);
    }

    col_datetime.Reset();
    EXPECT_EQ(col_datetime.capacity(), 0);
    EXPECT_EQ(col_datetime.tail_index_, 0);
    EXPECT_EQ(col_datetime.buffer_, nullptr);
    EXPECT_EQ(col_datetime.data_ptr_, nullptr);
    EXPECT_EQ(col_datetime.initialized, false);

    // ====
    col_datetime.Initialize();
    EXPECT_THROW(col_datetime.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_datetime.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_datetime.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_datetime.Size(), 0);
    EXPECT_THROW(col_datetime.ToString(), std::logic_error);
    EXPECT_THROW(col_datetime.GetValue(0), std::logic_error);
    EXPECT_EQ(col_datetime.tail_index_, 0);
    EXPECT_EQ(col_datetime.data_type_size_, 8);
    EXPECT_NE(col_datetime.data_ptr_, nullptr);
    EXPECT_EQ(col_datetime.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_datetime.data_type(), data_type);
    EXPECT_EQ(col_datetime.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_datetime.buffer_, nullptr);
    EXPECT_EQ(col_datetime.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_datetime.initialized);
    col_datetime.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_datetime.data_ptr_;
    EXPECT_EQ(col_datetime.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_datetime.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        DateTimeT datetime;
        datetime.date = static_cast<i32>(i);
        datetime.time = static_cast<i32>(i);
        Value v = Value::MakeDateTime(datetime);
        col_datetime.AppendValue(v);
        Value vx = col_datetime.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDateTime);
        EXPECT_EQ(vx.value_.datetime.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.datetime.time, static_cast<i32>(i));
        EXPECT_THROW(col_datetime.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_timestamp) {
    using namespace infinity;

    DataType data_type(LogicalType::kTimestamp);
    ColumnVector col_timestamp(data_type, ColumnVectorType::kFlat);
    col_timestamp.Initialize();

    EXPECT_THROW(col_timestamp.SetDataType(DataType(LogicalType::kTimestamp)), std::logic_error);
    EXPECT_THROW(col_timestamp.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_timestamp.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_timestamp.Size(), 0);
    EXPECT_THROW(col_timestamp.ToString(), std::logic_error);
    EXPECT_THROW(col_timestamp.GetValue(0), std::logic_error);
    EXPECT_EQ(col_timestamp.tail_index_, 0);
    EXPECT_EQ(col_timestamp.data_type_size_, 8);
    EXPECT_NE(col_timestamp.data_ptr_, nullptr);
    EXPECT_EQ(col_timestamp.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_timestamp.data_type(), data_type);
    EXPECT_EQ(col_timestamp.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_timestamp.buffer_, nullptr);
    EXPECT_EQ(col_timestamp.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_timestamp.initialized);
    col_timestamp.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_timestamp.data_ptr_;
    EXPECT_EQ(col_timestamp.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_timestamp.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        TimestampT timestamp;
        timestamp.date = static_cast<i32>(i);
        timestamp.time = static_cast<i32>(i);
        Value v = Value::MakeTimestamp(timestamp);
        col_timestamp.AppendValue(v);
        Value vx = col_timestamp.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestamp);
        EXPECT_EQ(vx.value_.timestamp.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp.time, static_cast<i32>(i));
        EXPECT_THROW(col_timestamp.GetValue(i + 1), std::logic_error);
    }

    col_timestamp.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_timestamp.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestamp);
        EXPECT_EQ(vx.value_.timestamp.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp.time, static_cast<i32>(i));
    }
    EXPECT_EQ(col_timestamp.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_timestamp.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        TimestampT timestamp;
        timestamp.date = static_cast<i32>(i);
        timestamp.time = static_cast<i32>(i);
        Value v = Value::MakeTimestamp(timestamp);
        col_timestamp.AppendValue(v);
        Value vx = col_timestamp.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestamp);
        EXPECT_EQ(vx.value_.timestamp.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp.time, static_cast<i32>(i));
        EXPECT_THROW(col_timestamp.GetValue(i + 1), std::logic_error);
    }

    col_timestamp.Reset();
    EXPECT_EQ(col_timestamp.capacity(), 0);
    EXPECT_EQ(col_timestamp.tail_index_, 0);
    EXPECT_EQ(col_timestamp.buffer_, nullptr);
    EXPECT_EQ(col_timestamp.data_ptr_, nullptr);
    EXPECT_EQ(col_timestamp.initialized, false);

    // ====
    col_timestamp.Initialize();
    EXPECT_THROW(col_timestamp.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_timestamp.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_timestamp.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_timestamp.Size(), 0);
    EXPECT_THROW(col_timestamp.ToString(), std::logic_error);
    EXPECT_THROW(col_timestamp.GetValue(0), std::logic_error);
    EXPECT_EQ(col_timestamp.tail_index_, 0);
    EXPECT_EQ(col_timestamp.data_type_size_, 8);
    EXPECT_NE(col_timestamp.data_ptr_, nullptr);
    EXPECT_EQ(col_timestamp.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_timestamp.data_type(), data_type);
    EXPECT_EQ(col_timestamp.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_timestamp.buffer_, nullptr);
    EXPECT_EQ(col_timestamp.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_timestamp.initialized);
    col_timestamp.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_timestamp.data_ptr_;
    EXPECT_EQ(col_timestamp.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_timestamp.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        TimestampT timestamp;
        timestamp.date = static_cast<i32>(i);
        timestamp.time = static_cast<i32>(i);
        Value v = Value::MakeTimestamp(timestamp);
        col_timestamp.AppendValue(v);
        Value vx = col_timestamp.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestamp);
        EXPECT_EQ(vx.value_.timestamp.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp.time, static_cast<i32>(i));
        EXPECT_THROW(col_timestamp.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_timestamp_tz) {
    using namespace infinity;

    DataType data_type(LogicalType::kTimestampTZ);
    ColumnVector col_timestamp_tz(data_type, ColumnVectorType::kFlat);
    col_timestamp_tz.Initialize();

    EXPECT_THROW(col_timestamp_tz.SetDataType(DataType(LogicalType::kTimestampTZ)), std::logic_error);
    EXPECT_THROW(col_timestamp_tz.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_timestamp_tz.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_timestamp_tz.Size(), 0);
    EXPECT_THROW(col_timestamp_tz.ToString(), std::logic_error);
    EXPECT_THROW(col_timestamp_tz.GetValue(0), std::logic_error);
    EXPECT_EQ(col_timestamp_tz.tail_index_, 0);
    EXPECT_EQ(col_timestamp_tz.data_type_size_, 8);
    EXPECT_NE(col_timestamp_tz.data_ptr_, nullptr);
    EXPECT_EQ(col_timestamp_tz.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_timestamp_tz.data_type(), data_type);
    EXPECT_EQ(col_timestamp_tz.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_timestamp_tz.buffer_, nullptr);
    EXPECT_EQ(col_timestamp_tz.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_timestamp_tz.initialized);
    col_timestamp_tz.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_timestamp_tz.data_ptr_;
    EXPECT_EQ(col_timestamp_tz.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_timestamp_tz.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        TimestampTZT timestamp_tz;
        timestamp_tz.date = static_cast<i32>(i);
        timestamp_tz.time = static_cast<i32>(i);
        Value v = Value::MakeTimestampTz(timestamp_tz);
        col_timestamp_tz.AppendValue(v);
        Value vx = col_timestamp_tz.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestampTZ);
        EXPECT_EQ(vx.value_.timestamp_tz.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp_tz.time, static_cast<i32>(i));
        EXPECT_THROW(col_timestamp_tz.GetValue(i + 1), std::logic_error);
    }

    col_timestamp_tz.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_timestamp_tz.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestampTZ);
        EXPECT_EQ(vx.value_.timestamp_tz.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp_tz.time, static_cast<i32>(i));
    }
    EXPECT_EQ(col_timestamp_tz.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_timestamp_tz.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        TimestampTZT timestamp_tz;
        timestamp_tz.date = static_cast<i32>(i);
        timestamp_tz.time = static_cast<i32>(i);
        Value v = Value::MakeTimestampTz(timestamp_tz);
        col_timestamp_tz.AppendValue(v);
        Value vx = col_timestamp_tz.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestampTZ);
        EXPECT_EQ(vx.value_.timestamp_tz.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp_tz.time, static_cast<i32>(i));
        EXPECT_THROW(col_timestamp_tz.GetValue(i + 1), std::logic_error);
    }

    col_timestamp_tz.Reset();
    EXPECT_EQ(col_timestamp_tz.capacity(), 0);
    EXPECT_EQ(col_timestamp_tz.tail_index_, 0);
    EXPECT_EQ(col_timestamp_tz.buffer_, nullptr);
    EXPECT_EQ(col_timestamp_tz.data_ptr_, nullptr);
    EXPECT_EQ(col_timestamp_tz.initialized, false);

    // ====
    col_timestamp_tz.Initialize();
    EXPECT_THROW(col_timestamp_tz.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_timestamp_tz.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_timestamp_tz.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_timestamp_tz.Size(), 0);
    EXPECT_THROW(col_timestamp_tz.ToString(), std::logic_error);
    EXPECT_THROW(col_timestamp_tz.GetValue(0), std::logic_error);
    EXPECT_EQ(col_timestamp_tz.tail_index_, 0);
    EXPECT_EQ(col_timestamp_tz.data_type_size_, 8);
    EXPECT_NE(col_timestamp_tz.data_ptr_, nullptr);
    EXPECT_EQ(col_timestamp_tz.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_timestamp_tz.data_type(), data_type);
    EXPECT_EQ(col_timestamp_tz.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_timestamp_tz.buffer_, nullptr);
    EXPECT_EQ(col_timestamp_tz.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_timestamp_tz.initialized);
    col_timestamp_tz.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_timestamp_tz.data_ptr_;
    EXPECT_EQ(col_timestamp_tz.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_timestamp_tz.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        TimestampTZT timestamp_tz;
        timestamp_tz.date = static_cast<i32>(i);
        timestamp_tz.time = static_cast<i32>(i);
        Value v = Value::MakeTimestampTz(timestamp_tz);
        col_timestamp_tz.AppendValue(v);
        Value vx = col_timestamp_tz.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTimestampTZ);
        EXPECT_EQ(vx.value_.timestamp_tz.date, static_cast<i32>(i));
        EXPECT_EQ(vx.value_.timestamp_tz.time, static_cast<i32>(i));
        EXPECT_THROW(col_timestamp_tz.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorTest, flat_interval) {
    using namespace infinity;

    DataType data_type(LogicalType::kInterval);
    ColumnVector col_interval(data_type, ColumnVectorType::kFlat);
    col_interval.Initialize();

    EXPECT_THROW(col_interval.SetDataType(DataType(LogicalType::kInterval)), std::logic_error);
    EXPECT_THROW(col_interval.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_interval.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_interval.Size(), 0);
    EXPECT_THROW(col_interval.ToString(), std::logic_error);
    EXPECT_THROW(col_interval.GetValue(0), std::logic_error);
    EXPECT_EQ(col_interval.tail_index_, 0);
    EXPECT_EQ(col_interval.data_type_size_, 8);
    EXPECT_NE(col_interval.data_ptr_, nullptr);
    EXPECT_EQ(col_interval.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_interval.data_type(), data_type);
    EXPECT_EQ(col_interval.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_interval.buffer_, nullptr);
    EXPECT_EQ(col_interval.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_interval.initialized);
    col_interval.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_interval.data_ptr_;
    EXPECT_EQ(col_interval.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_interval.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        IntervalT interval;
        interval.value = static_cast<i32>(i);
        Value v = Value::MakeInterval(interval);
        col_interval.AppendValue(v);
        Value vx = col_interval.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInterval);
        EXPECT_EQ(vx.value_.interval.value, static_cast<i32>(i));
        EXPECT_THROW(col_interval.GetValue(i + 1), std::logic_error);
    }

    col_interval.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_interval.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInterval);
        EXPECT_EQ(vx.value_.interval.value, static_cast<i32>(i));
    }
    EXPECT_EQ(col_interval.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_interval.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        IntervalT interval;
        interval.value = static_cast<i32>(i);
        Value v = Value::MakeInterval(interval);
        col_interval.AppendValue(v);
        Value vx = col_interval.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInterval);
        EXPECT_EQ(vx.value_.interval.value, static_cast<i32>(i));
        EXPECT_THROW(col_interval.GetValue(i + 1), std::logic_error);
    }

    col_interval.Reset();
    EXPECT_EQ(col_interval.capacity(), 0);
    EXPECT_EQ(col_interval.tail_index_, 0);
    EXPECT_EQ(col_interval.buffer_, nullptr);
    EXPECT_EQ(col_interval.data_ptr_, nullptr);
    EXPECT_EQ(col_interval.initialized, false);

    // ====
    col_interval.Initialize();
    EXPECT_THROW(col_interval.SetDataType(DataType(LogicalType::kTinyInt)), std::logic_error);
    EXPECT_THROW(col_interval.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_interval.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_interval.Size(), 0);
    EXPECT_THROW(col_interval.ToString(), std::logic_error);
    EXPECT_THROW(col_interval.GetValue(0), std::logic_error);
    EXPECT_EQ(col_interval.tail_index_, 0);
    EXPECT_EQ(col_interval.data_type_size_, 8);
    EXPECT_NE(col_interval.data_ptr_, nullptr);
    EXPECT_EQ(col_interval.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_interval.data_type(), data_type);
    EXPECT_EQ(col_interval.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_interval.buffer_, nullptr);
    EXPECT_EQ(col_interval.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_interval.initialized);
    col_interval.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_interval.data_ptr_;
    EXPECT_EQ(col_interval.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_interval.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        IntervalT interval;
        interval.value = static_cast<i32>(i);
        Value v = Value::MakeInterval(interval);
        col_interval.AppendValue(v);
        Value vx = col_interval.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kInterval);
        EXPECT_EQ(vx.value_.interval.value, static_cast<i32>(i));
        EXPECT_THROW(col_interval.GetValue(i + 1), std::logic_error);
    }
}
