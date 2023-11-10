// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "type/complex/embedding_type.h"
#include "type/complex/row_id.h"
#include "type/logical_type.h"
#include "type/type_info.h"

#include <memory>
#include <vector>

namespace infinity {

struct Value;

// Bool
using BooleanT = bool;

// Numeric
using TinyIntT = int8_t;
using SmallIntT = int16_t;
using IntegerT = int32_t;
using BigIntT = int64_t;
using HugeIntT = HugeInt;

using FloatT = float;
using DoubleT = double;

using DecimalT = DecimalType;

// std::string
using VarcharT = VarcharType;

// Date and Time
using DateT = DateType;
using TimeT = TimeType;
using DateTimeT = DateTimeType;
using TimestampT = TimestampType;
using IntervalT = IntervalType;

// Nest types
using ArrayT = std::vector<Value>;
using TupleT = std::vector<Value>;
// using ArrayT = ArrayType;
// using TupleT = TupleType;

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
using RowT = RowID;

// Heterogeneous
using MixedT = MixedType;

class DataType {
public:
    explicit DataType(LogicalType logical_type, std::shared_ptr<TypeInfo> type_info_ptr = nullptr);

    ~DataType() = default;

    DataType(const DataType &other) : type_(other.type_), plain_type_(other.plain_type_), type_info_(other.type_info_) {}

    DataType(DataType &&other) noexcept : type_(other.type_), plain_type_(other.plain_type_), type_info_(std::move(other.type_info_)) {}

    DataType &operator=(const DataType &other) {
        if (this == &other)
            return *this;
        type_ = other.type_;
        type_info_ = other.type_info_;
        plain_type_ = other.plain_type_;
        return *this;
    }

    DataType &operator=(DataType &&other) noexcept {
        if (this == &other)
            return *this;
        type_ = other.type_;
        type_info_ = other.type_info_;
        plain_type_ = other.plain_type_;
        other.type_info_ = nullptr;
        return *this;
    }

    bool operator==(const DataType &other) const;

    bool operator!=(const DataType &other) const;

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] size_t Size() const;

    [[nodiscard]] inline LogicalType type() const { return type_; }

    [[nodiscard]] inline const std::shared_ptr<TypeInfo> &type_info() const { return type_info_; }

    inline bool IsNumeric() const {
        switch (type_) {
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

    inline void Reset() {
        type_ = LogicalType::kInvalid;
        type_info_.reset();
        plain_type_ = false;
    }

    nlohmann::json Serialize();

    static std::shared_ptr<DataType> Deserialize(const nlohmann::json &data_type_json);

    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    [[nodiscard]] int32_t GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static std::shared_ptr<DataType> ReadAdv(char *&ptr, int32_t maxbytes);

    [[nodiscard]] inline bool Plain() const { return plain_type_; }

private:
    LogicalType type_{LogicalType::kInvalid};
    bool plain_type_{false};
    std::shared_ptr<TypeInfo> type_info_{nullptr};
    // Static method
public:
    template <typename T>
    static std::string TypeToString();

    template <typename T>
    static T StringToValue(const std::string_view &str_view);

    void MaxDataType(const DataType &right);
};

template <typename T>
std::string DataType::TypeToString() {
    ParserError("Unexpected date type.");
    return std::string();
}

template <typename T>
T DataType::StringToValue(const std::string_view &str_view) {
    ParserError("Unexpected data type.");
    return T();
}

template <>
std::string DataType::TypeToString<BooleanT>();

template <>
std::string DataType::TypeToString<TinyIntT>();

template <>
std::string DataType::TypeToString<SmallIntT>();

template <>
std::string DataType::TypeToString<IntegerT>();

template <>
std::string DataType::TypeToString<BigIntT>();

template <>
std::string DataType::TypeToString<HugeIntT>();

template <>
std::string DataType::TypeToString<FloatT>();

template <>
std::string DataType::TypeToString<DoubleT>();

template <>
std::string DataType::TypeToString<DecimalT>();

template <>
std::string DataType::TypeToString<VarcharT>();

template <>
std::string DataType::TypeToString<DateT>();

template <>
std::string DataType::TypeToString<TimeT>();

template <>
std::string DataType::TypeToString<DateTimeT>();

template <>
std::string DataType::TypeToString<TimestampT>();

template <>
std::string DataType::TypeToString<IntervalT>();

template <>
std::string DataType::TypeToString<ArrayT>();

// template <> std::string DataType::TypeToString<TupleT>() { return "Tuple"; }
template <>
std::string DataType::TypeToString<PointT>();

template <>
std::string DataType::TypeToString<LineT>();

template <>
std::string DataType::TypeToString<LineSegT>();

template <>
std::string DataType::TypeToString<BoxT>();

template <>
std::string DataType::TypeToString<PathT>();

template <>
std::string DataType::TypeToString<PolygonT>();

template <>
std::string DataType::TypeToString<CircleT>();

template <>
std::string DataType::TypeToString<BitmapT>();

template <>
std::string DataType::TypeToString<UuidT>();

template <>
std::string DataType::TypeToString<BlobT>();

template <>
std::string DataType::TypeToString<EmbeddingT>();

template <>
std::string DataType::TypeToString<RowT>();

template <>
std::string DataType::TypeToString<MixedT>();

template <>
BooleanT DataType::StringToValue<BooleanT>(const std::string_view &str_view);

template <>
TinyIntT DataType::StringToValue<TinyIntT>(const std::string_view &str_view);

template <>
SmallIntT DataType::StringToValue<SmallIntT>(const std::string_view &str_view);

template <>
IntegerT DataType::StringToValue<IntegerT>(const std::string_view &str_view);

template <>
BigIntT DataType::StringToValue<BigIntT>(const std::string_view &str_view);

template <>
FloatT DataType::StringToValue<FloatT>(const std::string_view &str_view);

template <>
DoubleT DataType::StringToValue<DoubleT>(const std::string_view &str_view);
} // namespace infinity
