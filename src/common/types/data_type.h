//
// Created by JinHai on 2022/10/27.
//

#pragma once

#include "sql/ColumnType.h"

#include "internal_types.h"
#include "type_info.h"

#include "decimal_type.h"
#include "huge_int.h"
#include "varchar_type.h"
#include "datetime_type.h"
#include "interval_type.h"

#include <string>

namespace infinity {

// Bool
using BooleanT = bool;

// Numeric
using TinyIntT = i8;
using SmallIntT = i16;
using IntegerT = i32;
using BigIntT = i64;
using HugeIntT = HugeInt;

using FloatT = float;
using DoubleT = double;

using DecimalT = DecimalType;

// String
using VarcharT = VarcharType;

// Date and Time
using DateT = i32;
using TimeT = i32;
using DateTimeT = DateTimeType;
using TimestampT = TimestampType;
using TimestampTZT = TimestampTZType;
using IntervalT = IntervalType;

// Nest types
// using ArrayT = ;
// using ObjectT = ;

// Geography
// using PointT = ;
// using LineT
// using LineSegT
// using BoxT
// using PathT
// using PolygonT
// using CircleT

// Other
// using BitmapT
// using UuidT
// using BlobT
// using VectorT
// using NullT

// 31 types in total now.
enum LogicalType: i8 {
    // Bool
    kBoolean = 0,

    // Numeric
    kTinyInt,
    kSmallInt,
    kInteger,
    kBigInt,
    kHugeInt,

    kFloat,
    kDouble,

    kDecimal,

    // String
    kVarchar,

    // Date and Time
    kDate,
    kTime,
    kDateTime,
    kTimestamp,
    kTimestampTZ,
    kInterval,

    // Nested types
    kArray,
    kObject,

    // Geography
    kPoint,
    kLine,
    kLineSeg,
    kBox,
    kPath,
    kPolygon,
    kCircle,

    // Other
    kBitmap,
    kUuid,
    kBlob,
    kVector,
    kNull,
    kMissing,

    kInvalid,
    // kAny, // Used for function?
};

enum class TypeFamily {

};

class DataType {
public:
    explicit
    DataType(LogicalType logical_type) : type_(logical_type) {};

    DataType(LogicalType logical_type, UniquePtr<TypeInfo> type_info_ptr) :
        type_(logical_type), type_info_(std::move(type_info_ptr)) {}

    bool
    operator==(const DataType& other) const;

    bool
    operator!=(const DataType& other) const;

    [[nodiscard]] std::string
    ToString() const;

    [[nodiscard]] size_t
    Width() const;

    [[nodiscard]] LogicalType
    type() const { return type_; }

private:

    LogicalType type_;
    UniquePtr<TypeInfo> type_info_;

    // Static method
public:
    static int64_t
    CastRule(const DataType& from, const DataType& to);

    static DataType
    ConvertType(hsql::ColumnType type);


};

}
