//
// Created by JinHai on 2022/10/27.
//

#pragma once

#include "sql/ColumnType.h"

#include "logical_type.h"
#include "type_info.h"

#include "internal_types.h"

#include "common/types/number/decimal_type.h"
#include "common/types/number/huge_int.h"
#include "common/types/complex/varchar_type.h"
#include "common/types/time/datetime_type.h"
#include "common/types/time/interval_type.h"
#include "common/types/complex/array_type.h"
#include "common/types/complex/object_type.h"
#include "common/types/complex/embedding_type.h"
#include "common/types/geo/point_type.h"
#include "common/types/geo/line_type.h"
#include "common/types/geo/line_segment_type.h"
#include "common/types/geo/box_type.h"
#include "common/types/geo/path_type.h"
#include "common/types/geo/polygon_type.h"
#include "common/types/geo/circle_type.h"
#include "common/types/complex/blob_type.h"
#include "common/types/complex/uuid.h"
#include "common/types/complex/bitmap_type.h"
#include "common/types/heterogenous/mixed_type.h"

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
using ArrayT = ArrayType;
using TupleT = TupleType;

// Geography
using PointT = PointType;
using LineT = LineType;
using LineSegT = LineSegmentType;
using BoxT = BoxType;
using PathT = PathType;
using PolygonT = PolygonType;
using CircleT = CircleType;

// Other
using BitmapT = BitmapType;
using UuidT = UuidType;
using BlobT = BlobType;
using EmbeddingT = EmbeddingType;

// Heterogeneous
using MixedT = DummyMixedType;


class DataType {
public:
    explicit
    DataType(LogicalType logical_type) : type_(logical_type) {};

    DataType(DataType&& other_type)  noexcept : type_(other_type.type_), type_info_(std::move(other_type.type_info_)) {}

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
    static i64
    CastRule(const DataType& from, const DataType& to);

    static DataType
    ConvertType(hsql::ColumnType type);

    static i64
    TypeSize(LogicalType logical_type);
};

}
