//
// Created by JinHai on 2022/10/30.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"

class DataTypeTest : public BaseTest {

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