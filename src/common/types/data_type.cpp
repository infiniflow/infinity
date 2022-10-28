//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "common/utility/infinity_assert.h"

#include "decimal_info.h"
#include "varchar_info.h"

namespace infinity {

std::string
DataType::ToString() const {
    switch(type_) {

        case LogicalType::kBoolean: return "Boolean";
        case LogicalType::kTinyInt: return "TinyInt";
        case LogicalType::kSmallInt: return "SmallInt";
        case LogicalType::kInteger: return "Integer";
        case LogicalType::kBigInt: return "BigInt";
        case LogicalType::kHugeInt: return "HugeInt";
        case LogicalType::kFloat: return "Float";
        case LogicalType::kDouble: return "Double";
        case LogicalType::kDecimal: return "Decimal";
        case LogicalType::kVarchar: return "Varchar";
        case LogicalType::kDate: return "Date";
        case LogicalType::kTime: return "Time";
        case LogicalType::kDateTime: return "DateTime";
        case LogicalType::kTimestamp: return "Timestamp";
        case LogicalType::kTimestampTZ: return "TimestampTZ";
        case LogicalType::kInterval: return "Interval";
        case LogicalType::kArray: return "Array";
        case LogicalType::kObject: return "Object";
        case LogicalType::kPoint: return "Point";
        case LogicalType::kLine: return "Line";
        case LogicalType::kLineSeg: return "LineSegment";
        case LogicalType::kBox: return "Box";
        case LogicalType::kPath: return "Path";
        case LogicalType::kPolygon: return "Polygon";
        case LogicalType::kCircle: return "Circle";
        case LogicalType::kBitmap: return "Bitmap";
        case LogicalType::kUuid: return "UUID";
        case LogicalType::kBlob: return "Blob";
        case LogicalType::kVector: return "Vector";
        default:
            StorageError("Invalid logical data type.");

    }
}

bool
DataType::operator==(const DataType &other) const {
    if(type_ != other.type_) return false;
    if(*this->type_info_ != *other.type_info_) return false;
    return true;
}

bool
DataType::operator!=(const DataType& other) const {
    return !operator==(other);
}

size_t
DataType::Width() const {
    switch(type_) {

        case LogicalType::kBoolean: return sizeof(BooleanT);
        case LogicalType::kTinyInt: return sizeof(TinyIntT);
        case LogicalType::kSmallInt: return sizeof(SmallIntT);
        case LogicalType::kInteger: return sizeof(IntervalT);
        case LogicalType::kBigInt: return sizeof(BigIntT);
        case LogicalType::kHugeInt: return sizeof(HugeIntT);
        case LogicalType::kFloat: return sizeof(FloatT);
        case LogicalType::kDouble: return sizeof(DoubleT);
        case LogicalType::kDecimal: return sizeof(DecimalT);
        case LogicalType::kVarchar: return sizeof(VarcharT);
        case LogicalType::kDate: return sizeof(DateT);
        case LogicalType::kTime: return sizeof(TimeT);
        case LogicalType::kDateTime: return sizeof(DateTimeT);
        case LogicalType::kTimestamp: return sizeof(TimestampT);
        case LogicalType::kTimestampTZ: return sizeof(TimestampTZT);
        case LogicalType::kInterval: return sizeof(IntervalT);
//        case LogicalType::kArray: return sizeof(ArrayT);
//        case LogicalType::kObject: return "Object";
//        case LogicalType::kPoint: return "Point";
//        case LogicalType::kLine: return "Line";
//        case LogicalType::kLineSeg: return "LineSegment";
//        case LogicalType::kBox: return "Box";
//        case LogicalType::kPath: return "Path";
//        case LogicalType::kPolygon: return "Polygon";
//        case LogicalType::kCircle: return "Circle";
//        case LogicalType::kBitmap: return "Bitmap";
//        case LogicalType::kUUID: return "UUID";
//        case LogicalType::kBlob: return "Blob";
//        case LogicalType::kVector: return "Vector";
        default:
            StorageError("Invalid data type.");

    }
}

int64_t
DataType::CastRule(const DataType &from, const DataType &to) {
    return 0;
}

DataType
DataType::ConvertType(hsql::ColumnType type) {
    switch (type.data_type) {
        case hsql::DataType::BOOLEAN: return DataType(LogicalType::kBoolean);
        // No TinyInt
        case hsql::DataType::SMALLINT: return DataType(LogicalType::kSmallInt);
        case hsql::DataType::INT: return DataType(LogicalType::kInteger);
        case hsql::DataType::BIGINT:
        case hsql::DataType::LONG: return DataType(LogicalType::kBigInt);;

        case hsql::DataType::DECIMAL: {
            UniquePtr<DecimalInfo> decimal_info = MakeUnique<DecimalInfo>();
            return DataType(LogicalType::kDecimal, std::move(decimal_info));
        }

        case hsql::DataType::REAL:
        case hsql::DataType::FLOAT: return DataType(LogicalType::kFloat);
        case hsql::DataType::DOUBLE: return DataType(LogicalType::kDouble);

        case hsql::DataType::CHAR:
        case hsql::DataType::VARCHAR:
        case hsql::DataType::TEXT: {
            UniquePtr<VarcharInfo> varchar_info = MakeUnique<VarcharInfo>();
            return DataType(LogicalType::kVarchar, std::move(varchar_info));
        }

        case hsql::DataType::DATE: return DataType(LogicalType::kDate);
        case hsql::DataType::TIME: return DataType(LogicalType::kTime);
        case hsql::DataType::DATETIME: return DataType(LogicalType::kDateTime);
        case hsql::DataType::UNKNOWN: {
            TypeError("Unknown date type.");
        }
    }
}

}


