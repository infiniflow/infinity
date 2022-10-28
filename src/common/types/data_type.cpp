//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "common/utility/infinity_assert.h"

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
        case LogicalType::kUUID: return "UUID";
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
DataType::Size() const {
    switch(type_) {

        case LogicalType::kBoolean: return sizeof(bool);
        case LogicalType::kTinyInt: return sizeof(i8);
        case LogicalType::kSmallInt: return sizeof(i16);
        case LogicalType::kInteger: return sizeof(i32);
        case LogicalType::kBigInt: return sizeof(i64);
//        case LogicalType::kHugeInt: return "HugeInt";
        case LogicalType::kFloat: return sizeof(float);
        case LogicalType::kDouble: return sizeof(double);
//        case LogicalType::kDecimal: return "Decimal";
//        case LogicalType::kVarchar: return "Varchar";
//        case LogicalType::kDate: return "Date";
//        case LogicalType::kTime: return "Time";
//        case LogicalType::kDateTime: return "DateTime";
//        case LogicalType::kTimestamp: return "Timestamp";
//        case LogicalType::kTimestampTZ: return "TimestampTZ";
//        case LogicalType::kInterval: return "Interval";
//        case LogicalType::kArray: return "Array";
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

}


