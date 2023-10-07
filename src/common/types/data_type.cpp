//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "common/types/info/decimal_info.h"
#include "common/types/info/embedding_info.h"
#include "common/types/info/varchar_info.h"
#include "common/types/logical_type.h"
#include "common/types/type_info.h"
#include "common/utility/infinity_assert.h"
#include "common/utility/serializable.h"
#include "function/cast/cast_table.h"
#include "info/bitmap_info.h"
#include <charconv>

namespace infinity {

String
DataType::ToString() const {
    if(type_ > kInvalid) {
        TypeError(fmt::format("Invalid logical data type {}.", int(type_)));
    }
    return LogicalType2Str(type_);
}

bool
DataType::operator==(const DataType& other) const {
    if(this == &other)
        return true;
    if(type_ != other.type_)
        return false;
    if(this->type_info_ == nullptr && other.type_info_ == nullptr) {
        return true;
    }
    if(this->type_info_ != nullptr && other.type_info_ != nullptr) {
        if(*this->type_info_ != *other.type_info_)
            return false;
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
        TypeError(fmt::format("Invalid logical data type {}.", int(type_)));
    }

    // embedding, varchar data can get data here.
    if(type_info_ != nullptr) {
        return type_info_->Size();
    }

    // StorageAssert(type_ != kEmbedding && type_ != kVarchar, "This ype should have type info");

    return LogicalTypeWidth(type_);
}

i64
DataType::CastRule(const DataType& from, const DataType& to) {
    return CastTable::instance().GetCastCost(from.type_, to.type_);
}

void
DataType::MaxDataType(const DataType& right) {
    if(*this == right) {
        return;
    }

    if(this->type_ == LogicalType::kInvalid) {
        *this = right;
        return;
    }

    if(right.type_ == LogicalType::kInvalid) {
        return;
    }

    if(this->IsNumeric() && right.IsNumeric()) {
        if(this->type_ > right.type_) {
            return;
        } else {
            *this = right;
            return;
        }
    }

    if(this->type_ == LogicalType::kVarchar) {
        return;
    }
    if(right.type_ == LogicalType::kVarchar) {
        *this = right;
        return;
    }

    if(this->type_ == LogicalType::kDateTime and right.type_ == LogicalType::kTimestamp) {
        *this = right;
        return;
    }

    if(this->type_ == LogicalType::kTimestamp and right.type_ == LogicalType::kDateTime) {
        return;
    }

    NotImplementError(fmt::format("Max type of left: {} and right: {}", this->ToString(), right.ToString()));
}

int32_t
DataType::GetSizeInBytes() const {
    int32_t size = sizeof(LogicalType);
    if(this->type_info_ != nullptr) {
        switch(this->type_) {
            case LogicalType::kArray:
                NotImplementError("Array isn't implemented here.");
                break;
            case LogicalType::kBitmap:
                size += sizeof(i64);
                break;
            case LogicalType::kDecimal:
                size += sizeof(i64) * 2;
                break;
            case LogicalType::kEmbedding:
                size += sizeof(EmbeddingDataType);
                size += sizeof(int32_t);
                break;
            case LogicalType::kVarchar:
                size += sizeof(int32_t);
                break;
            default:
                TypeError(
                        fmt::format("Unexpected type {} here.", int(this->type_)));
        }
    }
    return size;
}

void
DataType::WriteAdv(char*& ptr) const {
    WriteBufAdv<LogicalType>(ptr, this->type_);
    switch(this->type_) {
        case LogicalType::kArray:
            NotImplementError("Array isn't implemented here.");
            break;
        case LogicalType::kBitmap: {
            i64 limit = MAX_BITMAP_SIZE;
            if(this->type_info_ != nullptr) {
                const BitmapInfo* bitmap_info =
                        dynamic_cast<BitmapInfo*>(this->type_info_.get());
                if(bitmap_info != nullptr)
                    limit = bitmap_info->length_limit();
            }
            WriteBufAdv<i64>(ptr, limit);
            break;
        }
        case LogicalType::kDecimal: {
            i64 precision = 0;
            i64 scale = 0;
            if(this->type_info_ != nullptr) {
                const DecimalInfo* decimal_info =
                        dynamic_cast<DecimalInfo*>(this->type_info_.get());
                if(decimal_info != nullptr) {
                    precision = decimal_info->precision();
                    scale = decimal_info->scale();
                }
            }
            WriteBufAdv<i64>(ptr, precision);
            WriteBufAdv<i64>(ptr, scale);
            break;
        }
        case LogicalType::kEmbedding: {
            const EmbeddingInfo* embedding_info =
                    dynamic_cast<EmbeddingInfo*>(this->type_info_.get());
            TypeAssert(embedding_info != nullptr, fmt::format("kEmbedding associated type_info is nullptr here."));
            WriteBufAdv<EmbeddingDataType>(ptr, embedding_info->Type());
            WriteBufAdv<int32_t>(ptr, int32_t(embedding_info->Dimension()));
            break;
        }
        case LogicalType::kVarchar: {
            int32_t capacity = MAX_VARCHAR_SIZE;
            if(this->type_info_ != nullptr) {
                const VarcharInfo* varchar_info =
                        dynamic_cast<VarcharInfo*>(this->type_info_.get());
                if(varchar_info != nullptr)
                    capacity = varchar_info->dimension();
            }
            WriteBufAdv<int32_t>(ptr, capacity);
            break;
        }
        default:
            TypeError(fmt::format("Unexpected type {} here.", int(this->type_)));
    }
    return;
}

SharedPtr<DataType>
DataType::ReadAdv(char*& ptr, int32_t maxbytes) {
    char* const ptr_end = ptr + maxbytes;
    LogicalType type = ReadBufAdv<LogicalType>(ptr);
    SharedPtr<TypeInfo> type_info{nullptr};
    switch(type) {
        case LogicalType::kArray:
            NotImplementError("Array isn't implemented here.");
            break;
        case LogicalType::kBitmap: {
            i64 limit = ReadBufAdv<i64>(ptr);
            type_info = BitmapInfo::Make(limit);
            break;
        }
        case LogicalType::kDecimal: {
            i64 precision = ReadBufAdv<i64>(ptr);
            i64 scale = ReadBufAdv<i64>(ptr);
            type_info = DecimalInfo::Make(precision, scale);
            break;
        }
        case LogicalType::kEmbedding: {
            EmbeddingDataType embedding_type = ReadBufAdv<EmbeddingDataType>(ptr);
            int32_t dimension = ReadBufAdv<int32_t>(ptr);
            type_info = EmbeddingInfo::Make(EmbeddingDataType(embedding_type), dimension);
            break;
        }
        case LogicalType::kVarchar: {
            int32_t dimension = ReadBufAdv<int32_t>(ptr);
            type_info = VarcharInfo::Make(dimension);
            break;
        }
        default:
            TypeError(fmt::format("Unexpected type {} here.", int(type)));
    }
    maxbytes = ptr_end - ptr;
    StorageAssert(maxbytes >= 0,
                  "ptr goes out of range when reading DataType");
    SharedPtr<DataType> data_type = MakeShared<DataType>(type, type_info);
    return data_type;
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
    SharedPtr<TypeInfo> type_info{nullptr};
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
            case LogicalType::kVarchar: {
                type_info = VarcharInfo::Make(type_info_json["dimension"]);
                break;
            }
            default: {
                TypeError(fmt::format("Unexpected type {} here.", int(logical_type)));
            }
        }
    }
    SharedPtr<DataType> data_type = MakeShared<DataType>(logical_type, type_info);
    return data_type;
}

template<>
String
DataType::TypeToString<BooleanT>() { return "Boolean"; }

template<>
String
DataType::TypeToString<TinyIntT>() { return "TinyInt"; }

template<>
String
DataType::TypeToString<SmallIntT>() { return "SmallInt"; }

template<>
String
DataType::TypeToString<IntegerT>() { return "Integer"; }

template<>
String
DataType::TypeToString<BigIntT>() { return "BigInt"; }

template<>
String
DataType::TypeToString<HugeIntT>() { return "HugeInt"; }

template<>
String
DataType::TypeToString<FloatT>() { return "Float"; }

template<>
String
DataType::TypeToString<DoubleT>() { return "Double"; }

template<>
String
DataType::TypeToString<DecimalT>() { return "Decimal"; }

template<>
String
DataType::TypeToString<VarcharT>() { return "Varchar"; }

template<>
String
DataType::TypeToString<DateT>() { return "Date"; }

template<>
String
DataType::TypeToString<TimeT>() { return "Time"; }

template<>
String
DataType::TypeToString<DateTimeT>() { return "DateTime"; }

template<>
String
DataType::TypeToString<TimestampT>() { return "Timestamp"; }

template<>
String
DataType::TypeToString<IntervalT>() { return "Interval"; }

template<>
String
DataType::TypeToString<ArrayT>() { return "Array"; }

//template <> String DataType::TypeToString<TupleT>() { return "Tuple"; }
template<>
String
DataType::TypeToString<PointT>() { return "Point"; }

template<>
String
DataType::TypeToString<LineT>() { return "Line"; }

template<>
String
DataType::TypeToString<LineSegT>() { return "LineSegment"; }

template<>
String
DataType::TypeToString<BoxT>() { return "Box"; }

template<>
String
DataType::TypeToString<PathT>() { return "Path"; }

template<>
String
DataType::TypeToString<PolygonT>() { return "Polygon"; }

template<>
String
DataType::TypeToString<CircleT>() { return "Circle"; }

template<>
String
DataType::TypeToString<BitmapT>() { return "Bitmap"; }

template<>
String
DataType::TypeToString<UuidT>() { return "UUID"; }

template<>
String
DataType::TypeToString<BlobT>() { return "Blob"; }

template<>
String
DataType::TypeToString<EmbeddingT>() { return "Embedding"; }

template<>
String
DataType::TypeToString<RowT>() { return "RowID"; }

template<>
String
DataType::TypeToString<MixedT>() { return "Heterogeneous"; }

template<>
BooleanT
DataType::StringToValue<BooleanT>(const StringView& str) {
    if(str.empty()) {
        return BooleanT{};
    }
    // TODO: should support True/False, maybe others
    String str_lower;
    for(char ch: str) {
        str_lower.push_back(std::tolower(ch));
    }
    TypeAssert(str_lower == "true" || str_lower == "false", "Boolean type should be true or false");
    return str_lower == "true";
}

template<>
TinyIntT
DataType::StringToValue<TinyIntT>(const StringView& str) {
    if(str.empty()) {
        return TinyIntT{};
    }
    TinyIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse TinyInt error");// TODO: throw error here
    return value;
}

template<>
SmallIntT
DataType::StringToValue<SmallIntT>(const StringView& str) {
    if(str.empty()) {
        return SmallIntT{};
    }
    SmallIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse SmallInt error");
    return value;
}

template<>
IntegerT
DataType::StringToValue<IntegerT>(const StringView& str) {
    if(str.empty()) {
        return IntegerT{};
    }
    IntegerT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse Integer error");
    return value;
}

template<>
BigIntT
DataType::StringToValue<BigIntT>(const StringView& str) {
    if(str.empty()) {
        return BigIntT{};
    }
    BigIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse BigInt error");
    return value;
}

template<>
FloatT
DataType::StringToValue<FloatT>(const StringView& str) {
    if(str.empty()) {
        return FloatT{};
    }
    FloatT value{};
#if defined(__APPLE__)
    auto ret = std::sscanf(str.data(), "%a", &value);
    TypeAssert(ret == str.size(), "Parse Float error");
#else
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse Float error");
#endif
    return value;
}

template<>
DoubleT
DataType::StringToValue<DoubleT>(const StringView& str) {
    if(str.empty()) {
        return DoubleT{};
    }
    DoubleT value{};
#if defined(__APPLE__)
    auto ret = std::sscanf(str.data(), "%la", &value);
    TypeAssert(ret == str.size(), "Parse Double error");
#else
    auto res = std::from_chars(str.begin(), str.end(), value);
    TypeAssert(res.ptr == str.data() + str.size(), "Parse Double error");
#endif
    return value;
}
}// namespace infinity
