//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "function/cast/cast_table.h"
#include "common/utility/infinity_assert.h"

#include "common/types/info/decimal_info.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/embedding_info.h"
#include "common/types/info/char_info.h"

namespace infinity {

static const char* type2name[] = {
    // Bool
    "Boolean",

    // Numeric
    "TinyInt",
    "SmallInt",
    "Integer",
    "BigInt",
    "HugeInt",
    "Float",
    "Double",
    "Decimal16",
    "Decimal32",
    "Decimal64",
    "Decimal128",

    // String
    "Varchar",
    "Char",

    // Date and Time
    "Date",
    "Time",
    "DateTime",
    "Timestamp",
    "TimestampTZ",
    "Interval",

    // Nested types
    "Array",
    "Tuple",

    // Geography
    "Point",
    "Line",
    "LineSegment",
    "Box",
    "Path",
    "Polygon",
    "Circle",

    // Other
    "Bitmap",
    "UUID",
    "Blob",
    "Embedding",

    // Heterogeneous type
    "Heterogeneous",

    // only used in heterogeneous type
    "Null",
    "Missing",

    "Invalid",
};

static i64 type_size[] = {
    // Bool * 1
    1, // Boolean

    // Integer * 5
    1, // TinyInt
    2, // SmallInt
    4, // Integer
    8, // BigInt
    16, // HugeInt

    // Float * 2
    4, // Float
    8, // Double

    // Decimal * 4
    2, // Decimal16
    4, // Decimal32
    8, // Decimal64
    16, // Decimal128

    // Varchar * 1
    16, // Varchar

    // Char * 1
    8,  // Char

    // Date and Time * 6
    4, // Date
    4, // Time
    8, // DateTime
    8, // Timestamp
    8, // TimestampTZ
    8, // Interval

    // Nested types
    8, // Array
    4, // Tuple

    // Geography
    16, // Point
    24, // Line
    32, // LineSegment
    32, // Box
    16, // Path
    48, // Polygon
    24, // Circle

    // Other
    16, // Bitmap
    16, // UUID
    16, // Blob
    8, // Embedding

    // Heterogeneous
    16, // Mixed

    // only used in heterogeneous type
    0, // Null
    0, // Missing
    0, // Invalid
};

String
DataType::ToString() const {
    if(type_ > kInvalid) {
        TypeError("Invalid logical data type.");
    }
    return type2name[type_];
}

bool
DataType::operator==(const DataType &other) const {
    if(this == &other) return true;
    if(type_ != other.type_) return false;
    if(this->type_info_ == nullptr && other.type_info_ == nullptr) {
        return true;
    }
    if(this->type_info_ != nullptr && other.type_info_ != nullptr) {
        if(*this->type_info_ != *other.type_info_) return false;
        return true;
    } else {
        return false;
    }
}

bool
DataType::operator!=(const DataType& other) const {
    return !operator==(other);
}

size_t
DataType::Size() const {
    if(type_ > kInvalid) {
        StorageError("Invalid logical data type.");
    }

    if(type_info_ != nullptr) {
        return type_info_->Size();
    }

    return type_size[type_];
}

int64_t
DataType::CastRule(const DataType &from, const DataType &to) {
    return CastTable::instance().GetCastCost(from.type_, to.type_);
}

template <> String DataType::TypeToString<BooleanT>() { return "Boolean"; }
template <> String DataType::TypeToString<TinyIntT>() { return "TinyInt"; }
template <> String DataType::TypeToString<SmallIntT>() { return "SmallInt"; }
template <> String DataType::TypeToString<IntegerT>() { return "Integer"; }
template <> String DataType::TypeToString<BigIntT>() { return "BigInt"; }
template <> String DataType::TypeToString<HugeIntT>() { return "HugeInt"; }
template <> String DataType::TypeToString<FloatT>() { return "Float"; }
template <> String DataType::TypeToString<DoubleT>() { return "Double"; }
template <> String DataType::TypeToString<Decimal16T>() { return "Decimal16"; }
template <> String DataType::TypeToString<Decimal32T>() { return "Decimal32"; }
template <> String DataType::TypeToString<Decimal64T>() { return "Decimal64"; }
template <> String DataType::TypeToString<Decimal128T>() { return "Decimal128"; }
template <> String DataType::TypeToString<VarcharT>() { return "Varchar"; }
template <> String DataType::TypeToString<CharT>() { return "Char"; }
template <> String DataType::TypeToString<DateT>() { return "Date"; }
template <> String DataType::TypeToString<TimeT>() { return "Time"; }
template <> String DataType::TypeToString<DateTimeT>() { return "DateTime"; }
template <> String DataType::TypeToString<TimestampT>() { return "Timestamp"; }
template <> String DataType::TypeToString<TimestampTZT>() { return "TimestampTZ"; }
template <> String DataType::TypeToString<IntervalT>() { return "Interval"; }
template <> String DataType::TypeToString<ArrayT>() { return "Array"; }
//template <> String DataType::TypeToString<TupleT>() { return "Tuple"; }
template <> String DataType::TypeToString<PointT>() { return "Point"; }
template <> String DataType::TypeToString<LineT>() { return "Line"; }
template <> String DataType::TypeToString<LineSegT>() { return "LineSegment"; }
template <> String DataType::TypeToString<BoxT>() { return "Box"; }
template <> String DataType::TypeToString<PathT>() { return "Path"; }
template <> String DataType::TypeToString<PolygonT>() { return "Polygon"; }
template <> String DataType::TypeToString<CircleT>() { return "Circle"; }
template <> String DataType::TypeToString<BitmapT>() { return "Bitmap"; }
template <> String DataType::TypeToString<UuidT>() { return "UUID"; }
template <> String DataType::TypeToString<BlobT>() { return "Blob"; }
template <> String DataType::TypeToString<EmbeddingT>() { return "Embedding"; }
template <> String DataType::TypeToString<MixedT>() { return "Heterogeneous"; }

}


