//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "function/cast/cast_table.h"
#include "common/utility/infinity_assert.h"
#include "info/bitmap_info.h"
#include "common/types/info/decimal_info.h"
#include "common/types/info/embedding_info.h"
#include <charconv>

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
    "Decimal",
    "Float",
    "Double",

    // String
    "Varchar",

    // Date and Time
    "Date",
    "Time",
    "DateTime",
    "Timestamp",
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

    // Heterogeneous/Mix type
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

    // Decimal * 1
    16, // Decimal

    // Float * 2
    4, // Float
    8, // Double

    // Varchar * 1
    16, // Varchar

    // Date and Time * 6
    4, // Date
    4, // Time
    8, // DateTime
    8, // Timestamp
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

i64
DataType::CastRule(const DataType &from, const DataType &to) {
    return CastTable::instance().GetCastCost(from.type_, to.type_);
}

void
DataType::MaxDataType(const DataType& right) {
    if(*this == right) {
        return ;
    }

    if(this->type_ == LogicalType::kInvalid) {
        *this = right;
        return ;
    }

    if(right.type_ == LogicalType::kInvalid) {
        return ;
    }

    if(this->IsNumeric() && right.IsNumeric()) {
        if(this->type_ > right.type_) {
            return ;
        } else {
            *this = right;
            return ;
        }
    }

    if(this->type_ == LogicalType::kVarchar) {
        return ;
    }
    if(right.type_ == LogicalType::kVarchar) {
        *this = right;
        return ;
    }

    if(this->type_ == LogicalType::kDateTime and right.type_ == LogicalType::kTimestamp) {
        *this = right;
        return ;
    }

    if(this->type_ == LogicalType::kTimestamp and right.type_ == LogicalType::kDateTime) {
        return ;
    }

    NotImplementError(fmt::format("Max type of left: {} and right: {}", this->ToString(), right.ToString()));
}

nlohmann::json
DataType::Serialize() {
    nlohmann::json json_res;
    json_res["data_type"] = this->type_;

    if(this->type_info_ != nullptr) {
        json_res["type_info"] = this->type_info_->Serialize();
    }

    return json_res;
}

SharedPtr<DataType>
DataType::Deserialize(const nlohmann::json& data_type_json) {
    LogicalType logical_type = data_type_json["data_type"];
    SharedPtr<TypeInfo> type_info {nullptr};
    if(data_type_json.contains("type_info")) {
        const nlohmann::json& type_info_json = data_type_json["type_info"];
        switch(logical_type) {
            case LogicalType::kArray: {
                NotImplementError("Array isn't implemented here.");
                type_info = nullptr;
                break;
            }
            case LogicalType::kBitmap: {
                type_info = BitmapInfo::Make(type_info_json["length_limit"]);
                break;
            }
            case LogicalType::kDecimal: {
                type_info = DecimalInfo::Make(type_info_json["precision"], type_info_json["scale"]);
                break;
            }
            case LogicalType::kEmbedding: {
                type_info = EmbeddingInfo::Make(type_info_json["embedding_type"], type_info_json["dimension"]);
                break;
            }
            default: {
                TypeError("Unexpected type here.")
            }
        }
    }
    SharedPtr<DataType> data_type = MakeShared<DataType>(logical_type, type_info);
    return data_type;
}

template <> String DataType::TypeToString<BooleanT>() { return "Boolean"; }
template <> String DataType::TypeToString<TinyIntT>() { return "TinyInt"; }
template <> String DataType::TypeToString<SmallIntT>() { return "SmallInt"; }
template <> String DataType::TypeToString<IntegerT>() { return "Integer"; }
template <> String DataType::TypeToString<BigIntT>() { return "BigInt"; }
template <> String DataType::TypeToString<HugeIntT>() { return "HugeInt"; }
template <> String DataType::TypeToString<FloatT>() { return "Float"; }
template <> String DataType::TypeToString<DoubleT>() { return "Double"; }
template <> String DataType::TypeToString<DecimalT>() { return "Decimal"; }
template <> String DataType::TypeToString<VarcharT>() { return "Varchar"; }
template <> String DataType::TypeToString<DateT>() { return "Date"; }
template <> String DataType::TypeToString<TimeT>() { return "Time"; }
template <> String DataType::TypeToString<DateTimeT>() { return "DateTime"; }
template <> String DataType::TypeToString<TimestampT>() { return "Timestamp"; }
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

template <> BooleanT DataType::StringToValue<BooleanT>(const StringView& str) {
    if (str.empty()) {
        return BooleanT{};
    }
    // TODO: should support True/False, maybe others
    String str_lower;
    for (char ch : str) {
        str_lower.push_back(std::tolower(ch));
    }
    TypeAssert(str_lower == "true" || str_lower == "false","Boolean type should be true or false");
    return str_lower == "true";
}

template <> TinyIntT DataType::StringToValue<TinyIntT>(const StringView& str) {
    if (str.empty()) {
        return TinyIntT{};
    }
    TinyIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse TinyInt error"); // TODO: throw error here
    return value;
}

template <> SmallIntT DataType::StringToValue<SmallIntT>(const StringView& str) {
    if (str.empty()) {
        return SmallIntT{};
    }
    SmallIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse SmallInt error");
    return value;
}

template <> IntegerT DataType::StringToValue<IntegerT>(const StringView& str) {
    if (str.empty()) {
        return IntegerT{};
    }
    IntegerT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse Integer error");
    return value;
}

template <> BigIntT DataType::StringToValue<BigIntT>(const StringView& str) {
    if (str.empty()) {
        return BigIntT{};
    }
    BigIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse BigInt error");
    return value;
}

template <> FloatT DataType::StringToValue<FloatT>(const StringView& str) {
    if (str.empty()) {
        return FloatT{};
    }
    FloatT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse Float error");
    return value;
}

template <> DoubleT DataType::StringToValue<DoubleT>(const StringView& str) {
    if (str.empty()) {
        return DoubleT{};
    }
    DoubleT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse Double error");
    return value;
}
}


