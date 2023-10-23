//
// Created by JinHai on 2022/10/30.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity;

class DataTypeTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};


TEST_F(DataTypeTest, GetTypeName) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
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
    DataType decimal128_type(LogicalType::kDecimal);
    EXPECT_EQ(decimal128_type.ToString(), "Decimal");
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
    DataType row_id_type(LogicalType::kRowID);
    EXPECT_EQ(row_id_type.ToString(), "RowID");
    DataType null_type(LogicalType::kNull);
    EXPECT_EQ(null_type.ToString(), "Null");
    DataType missing_type(LogicalType::kMissing);
    EXPECT_EQ(missing_type.ToString(), "Missing");
}

TEST_F(DataTypeTest, TypeToString) {
//    using namespace infinity;
//    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
//    EXPECT_STREQ(DataType::TypeToString<BooleanT>().c_str(), "Boolean");
//    EXPECT_STREQ(DataType::TypeToString<TinyIntT>().c_str(), "TinyInt");
//    EXPECT_STREQ(DataType::TypeToString<SmallIntT>().c_str(), "SmallInt");
//    EXPECT_STREQ(DataType::TypeToString<IntegerT>().c_str(), "Integer");
//    EXPECT_STREQ(DataType::TypeToString<BigIntT>().c_str(), "BigInt");
//    EXPECT_STREQ(DataType::TypeToString<HugeIntT>().c_str(), "HugeInt");
//    EXPECT_STREQ(DataType::TypeToString<FloatT>().c_str(), "Float");
//    EXPECT_STREQ(DataType::TypeToString<DoubleT>().c_str(), "Double");
//    EXPECT_STREQ(DataType::TypeToString<DecimalT>().c_str(), "Decimal");
//    EXPECT_STREQ(DataType::TypeToString<VarcharT>().c_str(), "Varchar");
//    EXPECT_STREQ(DataType::TypeToString<DateT>().c_str(), "Date");
//    EXPECT_STREQ(DataType::TypeToString<TimeT>().c_str(), "Time");
//    EXPECT_STREQ(DataType::TypeToString<DateTimeT>().c_str(), "DateTime");
//    EXPECT_STREQ(DataType::TypeToString<TimestampT>().c_str(), "Timestamp");
//    EXPECT_STREQ(DataType::TypeToString<IntervalT>().c_str(), "Interval");
//    EXPECT_STREQ(DataType::TypeToString<ArrayT>().c_str(), "Array");
//    //    EXPECT_EQ(DataType::TypeToString<TupleT>().c_str(), "Tuple");
//    EXPECT_STREQ(DataType::TypeToString<PointT>().c_str(), "Point");
//    EXPECT_STREQ(DataType::TypeToString<LineT>().c_str(), "Line");
//    EXPECT_STREQ(DataType::TypeToString<LineSegT>().c_str(), "LineSegment");
//    EXPECT_STREQ(DataType::TypeToString<BoxT>().c_str(), "Box");
//    EXPECT_STREQ(DataType::TypeToString<PathT>().c_str(), "Path");
//    EXPECT_STREQ(DataType::TypeToString<PolygonT>().c_str(), "Polygon");
//    EXPECT_STREQ(DataType::TypeToString<CircleT>().c_str(), "Circle");
//    EXPECT_STREQ(DataType::TypeToString<BitmapT>().c_str(), "Bitmap");
//    EXPECT_STREQ(DataType::TypeToString<UuidT>().c_str(), "UUID");
//    EXPECT_STREQ(DataType::TypeToString<BlobT>().c_str(), "Blob");
//    EXPECT_STREQ(DataType::TypeToString<EmbeddingT>().c_str(), "Embedding");
//    EXPECT_STREQ(DataType::TypeToString<RowT>().c_str(), "RowID");
}

TEST_F(DataTypeTest, Serialize) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    DataType bool_type(LogicalType::kBoolean);
    String bool_type_str = bool_type.Serialize().dump();
    LOG_TRACE(Format("{}", bool_type_str));
}

TEST_F(DataTypeTest, ReadWrite) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<TypeInfo> type_info_bitmap = BitmapInfo::Make(1024);
    SharedPtr<TypeInfo> type_info_decimal = DecimalInfo::Make(i64(38), i64(3));
    SharedPtr<TypeInfo> type_info_embedding = EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 256);
    SharedPtr<TypeInfo> type_info_varchar = VarcharInfo::Make(8196);
    EXPECT_NE(type_info_bitmap, nullptr);
    EXPECT_NE(type_info_decimal, nullptr);
    EXPECT_NE(type_info_embedding, nullptr);
    EXPECT_NE(type_info_varchar, nullptr);

    std::vector<SharedPtr<DataType>> data_types = {
        MakeShared<DataType>(LogicalType::kTinyInt),
        MakeShared<DataType>(LogicalType::kFloat),
        MakeShared<DataType>(LogicalType::kTuple),
        MakeShared<DataType>(LogicalType::kBitmap, type_info_bitmap),
        MakeShared<DataType>(LogicalType::kDecimal, type_info_decimal),
        MakeShared<DataType>(LogicalType::kEmbedding, type_info_embedding),
        MakeShared<DataType>(LogicalType::kVarchar, type_info_varchar),
    };

    for (int i = 0; i < data_types.size(); i++) {
        SharedPtr<DataType> &data_type = data_types[i];
        const SharedPtr<TypeInfo> &ti = data_type->type_info();
        LOG_TRACE(data_type->Serialize().dump());
        int32_t exp_size = data_type->GetSizeInBytes();
        std::vector<char> buf(exp_size);
        char *buf_beg = buf.data();
        char *ptr = buf_beg;
        data_type->WriteAdv(ptr);
        EXPECT_EQ(ptr - buf_beg, exp_size);

        ptr = buf_beg;
        SharedPtr<DataType> data_type2 = DataType::ReadAdv(ptr, exp_size);
        EXPECT_NE(data_type2, nullptr);
        EXPECT_EQ(*data_type2, *data_type);
        EXPECT_EQ(ptr - buf_beg, exp_size);
    }
}
