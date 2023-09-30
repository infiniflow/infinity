//
// Created by JinHai on 2022/10/27.
//

#pragma once

#include "common/types/complex/embedding_type.h"
#include "common/utility/infinity_assert.h"
#include "logical_type.h"
#include "type_info.h"
#include "common/types/alias/containers.h"
#include "json.hpp"

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

using DecimalT = DecimalType;

// String
using VarcharT = VarcharType;

// Date and Time
using DateT = DateType;
using TimeT = TimeType;
using DateTimeT = DateTimeType;
using TimestampT = TimestampType;
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
    }

    explicit
    DataType(LogicalType logical_type) : type_(logical_type), type_info_(nullptr) {
    }

    ~DataType() = default;

    DataType(const DataType& other) : type_(other.type_), type_info_(other.type_info_) {
    }

    DataType(DataType&& other) noexcept: type_(other.type_), type_info_(std::move(other.type_info_)) {
    }

    DataType&
    operator=(const DataType& other) {
        if(this == &other)
            return *this;
        type_ = other.type_;
        type_info_ = other.type_info_;
        return *this;
    }

    DataType&
    operator=(DataType&& other) noexcept {
        if(this == &other)
            return *this;
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

    inline bool
    IsNumeric() const {
        switch(type_) {
            case kTinyInt:
            case kSmallInt:
            case kInteger:
            case kBigInt:
            case kHugeInt:
            case kFloat:
            case kDouble:
            case kDecimal: {
                return true;
            }
            default: {
                return false;
            }
        }
    }

    nlohmann::json
    Serialize();

    static SharedPtr<DataType>
    Deserialize(const nlohmann::json& data_type_json);

    friend class TableDef;

    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    int32_t
    GetSizeInBytes() const;

    // Write to a char buffer
    void
    WriteAdv(char*& buf) const;

    // Read from a serialized version
    static SharedPtr<DataType>
    ReadAdv(char*& buf, int32_t maxbytes);

private:

    LogicalType type_{LogicalType::kInvalid};
    SharedPtr<TypeInfo> type_info_{nullptr};

    // Static method
public:
    static i64
    CastRule(const DataType& from, const DataType& to);

    template<typename T>
    static String
    TypeToString();

    template<typename T>
    static T
    StringToValue(const StringView& str_view);

    void
    MaxDataType(const DataType& right);
};

template<typename T>
String
DataType::TypeToString() {
    TypeError("Unexpected date type.");
}

template<typename T>
T
DataType::StringToValue(const StringView& str_view) {
    TypeError("Unexpected data type.");
}

template<>
String
DataType::TypeToString<BooleanT>();

template<>
String
DataType::TypeToString<TinyIntT>();

template<>
String
DataType::TypeToString<SmallIntT>();

template<>
String
DataType::TypeToString<IntegerT>();

template<>
String
DataType::TypeToString<BigIntT>();

template<>
String
DataType::TypeToString<HugeIntT>();

template<>
String
DataType::TypeToString<FloatT>();

template<>
String
DataType::TypeToString<DoubleT>();

template<>
String
DataType::TypeToString<DecimalT>();

template<>
String
DataType::TypeToString<VarcharT>();

template<>
String
DataType::TypeToString<DateT>();

template<>
String
DataType::TypeToString<TimeT>();

template<>
String
DataType::TypeToString<DateTimeT>();

template<>
String
DataType::TypeToString<TimestampT>();

template<>
String
DataType::TypeToString<IntervalT>();

template<>
String
DataType::TypeToString<ArrayT>();

//template <> String DataType::TypeToString<TupleT>() { return "Tuple"; }
template<>
String
DataType::TypeToString<PointT>();

template<>
String
DataType::TypeToString<LineT>();

template<>
String
DataType::TypeToString<LineSegT>();

template<>
String
DataType::TypeToString<BoxT>();

template<>
String
DataType::TypeToString<PathT>();

template<>
String
DataType::TypeToString<PolygonT>();

template<>
String
DataType::TypeToString<CircleT>();

template<>
String
DataType::TypeToString<BitmapT>();

template<>
String
DataType::TypeToString<UuidT>();

template<>
String
DataType::TypeToString<BlobT>();

template<>
String
DataType::TypeToString<EmbeddingT>();

template<>
String
DataType::TypeToString<MixedT>();

template<>
BooleanT
DataType::StringToValue<BooleanT>(const StringView& str_view);

template<>
TinyIntT
DataType::StringToValue<TinyIntT>(const StringView& str_view);

template<>
SmallIntT
DataType::StringToValue<SmallIntT>(const StringView& str_view);

template<>
IntegerT
DataType::StringToValue<IntegerT>(const StringView& str_view);

template<>
BigIntT
DataType::StringToValue<BigIntT>(const StringView& str_view);

template<>
FloatT
DataType::StringToValue<FloatT>(const StringView& str_view);

template<>
DoubleT
DataType::StringToValue<DoubleT>(const StringView& str_view);
}
