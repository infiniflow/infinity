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
#include "common/types/complex/char_type.h"
#include "common/types/datetime/datetime_type.h"
#include "common/types/datetime/interval_type.h"
#include "common/types/datetime/date_type.h"
#include "common/types/datetime/time_type.h"
#include "common/types/datetime/timestamp_type.h"
#include "common/types/datetime/timestamp_tz_type.h"
#include "common/types/complex/embedding_type.h"
#include "common/types/geo/point_type.h"
#include "common/types/geo/line_type.h"
#include "common/types/geo/line_segment_type.h"
#include "common/types/geo/box_type.h"
#include "common/types/geo/path_type.h"
#include "common/types/geo/polygon_type.h"
#include "common/types/geo/circle_type.h"
#include "common/types/complex/blob_type.h"
#include "common/types/complex/uuid_type.h"
#include "common/types/complex/bitmap_type.h"
#include "common/types/heterogenous/mixed_type.h"

#include <string>

namespace infinity {

struct Value;

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

using Decimal16T = Decimal16Type;
using Decimal32T = Decimal32Type;
using Decimal64T = Decimal64Type;
using Decimal128T = Decimal128Type;

// String
using VarcharT = VarcharType;
using Char1T = Char1Type;
using Char2T = Char2Type;
using Char4T = Char4Type;
using Char8T = Char8Type;
using Char16T = Char16Type;
using Char32T = Char32Type;
using Char64T = Char64Type;

// Date and Time
using DateT = DateType;
using TimeT = TimeType;
using DateTimeT = DateTimeType;
using TimestampT = TimestampType;
using TimestampTZT = TimestampTZType;
using IntervalT = IntervalType;

// Nest types
using ArrayT = Vector<Value>;
using TupleT = Vector<Value>;
//using ArrayT = ArrayType;
//using TupleT = TupleType;

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
using MixedT = MixedType;


class DataType {
public:
    explicit
    DataType(LogicalType logical_type, SharedPtr<TypeInfo> type_info_ptr) :
        type_(logical_type), type_info_(std::move(type_info_ptr)) {
        GlobalResourceUsage::IncrObjectCount();
    }

    explicit
    DataType(LogicalType logical_type): type_(logical_type), type_info_(nullptr) {
        GlobalResourceUsage::IncrObjectCount();
    }

    ~DataType() {
        GlobalResourceUsage::DecrObjectCount();
    }

    DataType(const DataType& other): type_(other.type_), type_info_(other.type_info_) {
        GlobalResourceUsage::IncrObjectCount();
    }

    DataType(DataType&& other) noexcept: type_(other.type_), type_info_(std::move(other.type_info_)) {
        GlobalResourceUsage::IncrObjectCount();
    }

    DataType&
    operator=(const DataType& other) {
        if(this == &other) return *this;
        type_ = other.type_;
        type_info_ = other.type_info_;
        return *this;
    }

    DataType&
    operator=(DataType&& other) noexcept {
        if(this == &other) return *this;
        type_ = other.type_;
        type_info_ = other.type_info_;
        other.type_info_ = nullptr;
        return *this;
    }

    bool
    operator==(const DataType& other) const;

    bool
    operator!=(const DataType& other) const;

    [[nodiscard]] std::string
    ToString() const;

    [[nodiscard]] size_t
    Size() const;

    [[nodiscard]] inline LogicalType
    type() const { return type_; }

    [[nodiscard]] inline const SharedPtr<TypeInfo>&
    type_info() const { return type_info_; }

    inline void
    Reset() {
        type_ = LogicalType::kInvalid;
        type_info_.reset();
    }

private:

    LogicalType type_{LogicalType::kInvalid};
    SharedPtr<TypeInfo> type_info_{nullptr};

    // Static method
public:
    static i64
    CastRule(const DataType& from, const DataType& to);

    static DataType
    ConvertType(hsql::ColumnType type);

    template<typename T>
    static String
    TypeToString();
};

template<typename T>
String
DataType::TypeToString() {
    TypeError("Unexpected date type.");
}

template <> String DataType::TypeToString<BooleanT>();
template <> String DataType::TypeToString<TinyIntT>();
template <> String DataType::TypeToString<SmallIntT>();
template <> String DataType::TypeToString<IntegerT>();
template <> String DataType::TypeToString<BigIntT>();
template <> String DataType::TypeToString<HugeIntT>();
template <> String DataType::TypeToString<FloatT>();
template <> String DataType::TypeToString<DoubleT>();
template <> String DataType::TypeToString<Decimal16T>();
template <> String DataType::TypeToString<Decimal32T>();
template <> String DataType::TypeToString<Decimal64T>();
template <> String DataType::TypeToString<Decimal128T>();
template <> String DataType::TypeToString<VarcharT>();
template <> String DataType::TypeToString<Char1T>();
template <> String DataType::TypeToString<Char2T>();
template <> String DataType::TypeToString<Char4T>();
template <> String DataType::TypeToString<Char8T>();
template <> String DataType::TypeToString<Char16T>();
template <> String DataType::TypeToString<Char32T>();
template <> String DataType::TypeToString<Char64T>();
template <> String DataType::TypeToString<DateT>();
template <> String DataType::TypeToString<TimeT>();
template <> String DataType::TypeToString<DateTimeT>();
template <> String DataType::TypeToString<TimestampT>();
template <> String DataType::TypeToString<TimestampTZT>();
template <> String DataType::TypeToString<IntervalT>();
template <> String DataType::TypeToString<ArrayT>();
//template <> String DataType::TypeToString<TupleT>() { return "Tuple"; }
template <> String DataType::TypeToString<PointT>();
template <> String DataType::TypeToString<LineT>();
template <> String DataType::TypeToString<LineSegT>();
template <> String DataType::TypeToString<BoxT>();
template <> String DataType::TypeToString<PathT>();
template <> String DataType::TypeToString<PolygonT>();
template <> String DataType::TypeToString<CircleT>();
template <> String DataType::TypeToString<BitmapT>();
template <> String DataType::TypeToString<UuidT>();
template <> String DataType::TypeToString<BlobT>();
template <> String DataType::TypeToString<EmbeddingT>();
template <> String DataType::TypeToString<MixedT>();

}
