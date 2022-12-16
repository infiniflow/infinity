//
// Created by JinHai on 2022/10/30.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class DataTypeTest : public BaseTest {
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

TEST_F(DataTypeTest, GetTypeName) {
    using namespace infinity;
    DataType bool_type(LogicalType::kBoolean);
    EXPECT_EQ(bool_type.ToString(), "Boolean");
    DataType tinyint_type(LogicalType::kTinyInt);
    EXPECT_EQ(tinyint_type.ToString(), "TinyInt");
    DataType smallint_type(LogicalType::kSmallInt);
    EXPECT_EQ(smallint_type.ToString(), "SmallInt");
    DataType integer_type(LogicalType::kInteger);
    EXPECT_EQ(integer_type.ToString(), "Integer");
    DataType bigint_type(LogicalType::kBigInt);
    EXPECT_EQ(bigint_type.ToString(), "BigInt");
    DataType hugeint_type(LogicalType::kHugeInt);
    EXPECT_EQ(hugeint_type.ToString(), "HugeInt");
    DataType float_type(LogicalType::kFloat);
    EXPECT_EQ(float_type.ToString(), "Float");
    DataType double_type(LogicalType::kDouble);
    EXPECT_EQ(double_type.ToString(), "Double");
    DataType decimal16_type(LogicalType::kDecimal16);
    EXPECT_EQ(decimal16_type.ToString(), "Decimal16");
    DataType decimal32_type(LogicalType::kDecimal32);
    EXPECT_EQ(decimal32_type.ToString(), "Decimal32");
    DataType decimal64_type(LogicalType::kDecimal64);
    EXPECT_EQ(decimal64_type.ToString(), "Decimal64");
    DataType decimal128_type(LogicalType::kDecimal128);
    EXPECT_EQ(decimal128_type.ToString(), "Decimal128");
    DataType varchar_type(LogicalType::kVarchar);
    EXPECT_EQ(varchar_type.ToString(), "Varchar");
    DataType char1_type(LogicalType::kChar1);
    EXPECT_EQ(char1_type.ToString(), "Char1");
    DataType char2_type(LogicalType::kChar2);
    EXPECT_EQ(char2_type.ToString(), "Char2");
    DataType char4_type(LogicalType::kChar4);
    EXPECT_EQ(char4_type.ToString(), "Char4");
    DataType char8_type(LogicalType::kChar8);
    EXPECT_EQ(char8_type.ToString(), "Char8");
    DataType char16_type(LogicalType::kChar16);
    EXPECT_EQ(char16_type.ToString(), "Char16");
    DataType char32_type(LogicalType::kChar32);
    EXPECT_EQ(char32_type.ToString(), "Char32");
    DataType char64_type(LogicalType::kChar64);
    EXPECT_EQ(char64_type.ToString(), "Char64");
    DataType date_type(LogicalType::kDate);
    EXPECT_EQ(date_type.ToString(), "Date");
    DataType time_type(LogicalType::kTime);
    EXPECT_EQ(time_type.ToString(), "Time");
    DataType datetime_type(LogicalType::kDateTime);
    EXPECT_EQ(datetime_type.ToString(), "DateTime");
    DataType timestamp_type(LogicalType::kTimestamp);
    EXPECT_EQ(timestamp_type.ToString(), "Timestamp");
    DataType timestamptz_type(LogicalType::kTimestampTZ);
    EXPECT_EQ(timestamptz_type.ToString(), "TimestampTZ");
    DataType interval_type(LogicalType::kInterval);
    EXPECT_EQ(interval_type.ToString(), "Interval");
    DataType array_type(LogicalType::kArray);
    EXPECT_EQ(array_type.ToString(), "Array");
    DataType object_type(LogicalType::kTuple);
    EXPECT_EQ(object_type.ToString(), "Tuple");
    DataType point_type(LogicalType::kPoint);
    EXPECT_EQ(point_type.ToString(), "Point");
    DataType line_type(LogicalType::kLine);
    EXPECT_EQ(line_type.ToString(), "Line");
    DataType line_seg_type(LogicalType::kLineSeg);
    EXPECT_EQ(line_seg_type.ToString(), "LineSegment");
    DataType box_type(LogicalType::kBox);
    EXPECT_EQ(box_type.ToString(), "Box");
    DataType path_type(LogicalType::kPath);
    EXPECT_EQ(path_type.ToString(), "Path");
    DataType polygon(LogicalType::kPolygon);
    EXPECT_EQ(polygon.ToString(), "Polygon");
    DataType circle_type(LogicalType::kCircle);
    EXPECT_EQ(circle_type.ToString(), "Circle");
    DataType bitmap_type(LogicalType::kBitmap);
    EXPECT_EQ(bitmap_type.ToString(), "Bitmap");
    DataType uuid_type(LogicalType::kUuid);
    EXPECT_EQ(uuid_type.ToString(), "UUID");
    DataType blob_type(LogicalType::kBlob);
    EXPECT_EQ(blob_type.ToString(), "Blob");
    DataType vector_type(LogicalType::kEmbedding);
    EXPECT_EQ(vector_type.ToString(), "Embedding");
    DataType null_type(LogicalType::kNull);
    EXPECT_EQ(null_type.ToString(), "Null");
    DataType missing_type(LogicalType::kMissing);
    EXPECT_EQ(missing_type.ToString(), "Missing");

}

TEST_F(DataTypeTest, TypeToString) {
    using namespace infinity;
    EXPECT_STREQ(DataType::TypeToString<BooleanT>().c_str(), "Boolean");
    EXPECT_STREQ(DataType::TypeToString<TinyIntT>().c_str(), "TinyInt");
    EXPECT_STREQ(DataType::TypeToString<SmallIntT>().c_str(), "SmallInt");
    EXPECT_STREQ(DataType::TypeToString<IntegerT>().c_str(), "Integer");
    EXPECT_STREQ(DataType::TypeToString<BigIntT>().c_str(), "BigInt");
    EXPECT_STREQ(DataType::TypeToString<HugeIntT>().c_str(), "HugeInt");
    EXPECT_STREQ(DataType::TypeToString<FloatT>().c_str(), "Float");
    EXPECT_STREQ(DataType::TypeToString<DoubleT>().c_str(), "Double");
    EXPECT_STREQ(DataType::TypeToString<Decimal16T>().c_str(), "Decimal16");
    EXPECT_STREQ(DataType::TypeToString<Decimal32T>().c_str(), "Decimal32");
    EXPECT_STREQ(DataType::TypeToString<Decimal64T>().c_str(), "Decimal64");
    EXPECT_STREQ(DataType::TypeToString<Decimal128T>().c_str(), "Decimal128");
    EXPECT_STREQ(DataType::TypeToString<VarcharT>().c_str(), "Varchar");
    EXPECT_STREQ(DataType::TypeToString<Char1T>().c_str(), "Char1");
    EXPECT_STREQ(DataType::TypeToString<Char2T>().c_str(), "Char2");
    EXPECT_STREQ(DataType::TypeToString<Char4T>().c_str(), "Char4");
    EXPECT_STREQ(DataType::TypeToString<Char8T>().c_str(), "Char8");
    EXPECT_STREQ(DataType::TypeToString<Char16T>().c_str(), "Char16");
    EXPECT_STREQ(DataType::TypeToString<Char32T>().c_str(), "Char32");
    EXPECT_STREQ(DataType::TypeToString<Char64T>().c_str(), "Char64");
    EXPECT_STREQ(DataType::TypeToString<DateT>().c_str(), "Date");
    EXPECT_STREQ(DataType::TypeToString<TimeT>().c_str(), "Time");
    EXPECT_STREQ(DataType::TypeToString<DateTimeT>().c_str(), "DateTime");
    EXPECT_STREQ(DataType::TypeToString<TimestampT>().c_str(), "Timestamp");
    EXPECT_STREQ(DataType::TypeToString<TimestampTZT>().c_str(), "TimestampTZ");
    EXPECT_STREQ(DataType::TypeToString<IntervalT>().c_str(), "Interval");
    EXPECT_STREQ(DataType::TypeToString<ArrayT>().c_str(), "Array");
//    EXPECT_EQ(DataType::TypeToString<TupleT>().c_str(), "Tuple");
    EXPECT_STREQ(DataType::TypeToString<PointT>().c_str(), "Point");
    EXPECT_STREQ(DataType::TypeToString<LineT>().c_str(), "Line");
    EXPECT_STREQ(DataType::TypeToString<LineSegT>().c_str(), "LineSegment");
    EXPECT_STREQ(DataType::TypeToString<BoxT>().c_str(), "Box");
    EXPECT_STREQ(DataType::TypeToString<PathT>().c_str(), "Path");
    EXPECT_STREQ(DataType::TypeToString<PolygonT>().c_str(), "Polygon");
    EXPECT_STREQ(DataType::TypeToString<CircleT>().c_str(), "Circle");
    EXPECT_STREQ(DataType::TypeToString<BitmapT>().c_str(), "Bitmap");
    EXPECT_STREQ(DataType::TypeToString<UuidT>().c_str(), "UUID");
    EXPECT_STREQ(DataType::TypeToString<BlobT>().c_str(), "Blob");
    EXPECT_STREQ(DataType::TypeToString<EmbeddingT>().c_str(), "Embedding");
}