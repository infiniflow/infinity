//
// Created by JinHai on 2022/9/9.
//
#if 0
#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/vector_operations_old.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class VectorOperationTest : public BaseTest {
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
    }
};

TEST_F(VectorOperationTest, BoolToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk source_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        source_bool.Append(true);
        source_bool.Append(false);

        // To bool
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "true\nfalse\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bool, target_text);
        EXPECT_EQ(target_text.ToString(), "true\nfalse\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, tinyintToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk source_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        source_tinyint.Append(static_cast<TinyIntT>(1));
        source_tinyint.Append(static_cast<TinyIntT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
//        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_tinyint, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, SmallintoToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk source_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        source_smallint.Append(static_cast<SmallIntT>(1));
        source_smallint.Append(static_cast<SmallIntT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
//        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_smallint, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, IntToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk source_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        source_int.Append(static_cast<IntegerT>(1));
        source_int.Append(static_cast<IntegerT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
//        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_int, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, BigintToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk source_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        source_bigint.Append(static_cast<BigIntT>(1));
        source_bigint.Append(static_cast<BigIntT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
//        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_bigint, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, DateToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk source_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        source_date.Append(static_cast<DateT>(1));
        source_date.Append(static_cast<DateT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
//        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_date, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, TimeToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk source_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        source_time.Append(static_cast<DateT>(1));
        source_time.Append(static_cast<DateT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
//        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_time, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, DateTimeToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk source_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        source_datetime.Append(static_cast<DateTimeT>(1));
        source_datetime.Append(static_cast<DateTimeT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
//        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_datetime, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}


TEST_F(VectorOperationTest, IntervalToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk source_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        source_interval.Append(static_cast<IntervalT>(1));
        source_interval.Append(static_cast<IntervalT>(0));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_bool);
        EXPECT_EQ(target_bool.ToString(), "true\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
//        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_interval, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, VarcharToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk source_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        source_varchar.Append(static_cast<VarcharT>("1"));
        source_varchar.Append(static_cast<VarcharT>("0"));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_bool);
        EXPECT_EQ(target_bool.ToString(), "false\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
//        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_varchar, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}

TEST_F(VectorOperationTest, TextToOther) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    try {
        // Create bool chunk with 2 rows;
        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk source_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        source_text.Append(static_cast<TextT>("1"));
        source_text.Append(static_cast<TextT>("0"));

        // To bool
        auto bool_type = LogicalType(LogicalTypeId::kBoolean);
        Chunk target_bool(bool_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_bool);
        EXPECT_EQ(target_bool.ToString(), "false\nfalse\n");

        // To tinyint
        auto tinyint_type = LogicalType(LogicalTypeId::kTinyInt);
        Chunk target_tinyint(tinyint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_tinyint);
        EXPECT_EQ(target_tinyint.ToString(), "1\n0\n");

        // To smallint
        auto smallint_type = LogicalType(LogicalTypeId::kSmallInt);
        Chunk target_smallint(smallint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_smallint);
        EXPECT_EQ(target_smallint.ToString(), "1\n0\n");

        // To int
        auto int_type = LogicalType(LogicalTypeId::kInteger);
        Chunk target_int(int_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_int);
        EXPECT_EQ(target_int.ToString(), "1\n0\n");

        // To bigint
        auto bigint_type = LogicalType(LogicalTypeId::kBigInt);
        Chunk target_bigint(bigint_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_bigint);
        EXPECT_EQ(target_bigint.ToString(), "1\n0\n");

        // To Date
        auto date_type = LogicalType(LogicalTypeId::kDate);
        Chunk target_date(date_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_date);
        EXPECT_EQ(target_date.ToString(), "1\n0\n");

        // To Time
        auto time_type = LogicalType(LogicalTypeId::kTime);
        Chunk target_time(time_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_time);
        EXPECT_EQ(target_time.ToString(), "1\n0\n");

        // To DateTime
        auto datetime_type = LogicalType(LogicalTypeId::kDateTime);
        Chunk target_datetime(datetime_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_datetime);
        EXPECT_EQ(target_datetime.ToString(), "1\n0\n");

        // To Interval
        auto interval_type = LogicalType(LogicalTypeId::kInterval);
        Chunk target_interval(interval_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_interval);
        EXPECT_EQ(target_interval.ToString(), "1\n0\n");

        // To Varchar
        auto varchar_type = LogicalType(LogicalTypeId::kVarchar);
        Chunk target_varchar(varchar_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_varchar);
        EXPECT_EQ(target_varchar.ToString(), "1\n0\n");

        // To Text
//        auto text_type = LogicalType(LogicalTypeId::kText);
        Chunk target_text(text_type, Infinity::instance().config()->option_.default_row_count_);
        VectorOperation::VectorCast(source_text, target_text);
        EXPECT_EQ(target_text.ToString(), "1\n0\n");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Cast Error");
    }
}
#endif
