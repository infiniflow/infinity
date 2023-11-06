//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "info/bitmap_info.h"
#include "serializable.h"
#include "spdlog/fmt/fmt.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"
#include "type/info/varchar_info.h"
#include "type/logical_type.h"
#include "type/type_info.h"
#include <charconv>

namespace infinity {

DataType::DataType(LogicalType logical_type, std::shared_ptr<TypeInfo> type_info_ptr) : type_(logical_type), type_info_(std::move(type_info_ptr)) {
    switch (logical_type) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
        case kBitmap:
        case kUuid:
        case kEmbedding:
        case kRowID: {
            plain_type_ = true;
            break;
        }
        case kMixed:
        case kVarchar:
        case kArray:
        case kTuple:
        case kPath:
        case kPolygon:
        case kBlob: {
            plain_type_ = false;
            break;
        }
        case kNull:
        case kMissing: {
            plain_type_ = true;
            break;
        }
        case kInvalid:
            break;
    }
}

std::string DataType::ToString() const {
    if (type_ > kInvalid) {
        ParserError(fmt::format("Invalid logical data type {}.", int(type_)));
    }
    return LogicalType2Str(type_);
}

bool DataType::operator==(const DataType &other) const {
    if (this == &other)
        return true;
    if (type_ != other.type_)
        return false;
    if (plain_type_ != other.plain_type_)
        return false;
    if (this->type_info_ == nullptr && other.type_info_ == nullptr) {
        return true;
    }
    if (this->type_info_ != nullptr && other.type_info_ != nullptr) {
        if (*this->type_info_ != *other.type_info_)
            return false;
        else
            return true;
    } else {
        return false;
    }
}

bool DataType::operator!=(const DataType &other) const { return !operator==(other); }

size_t DataType::Size() const {
    if (type_ > kInvalid) {
        ParserError(fmt::format("Invalid logical data type {}.", int(type_)));
    }

    // embedding, varchar data can get data here.
    if (type_info_ != nullptr) {
        return type_info_->Size();
    }

    // StorageAssert(type_ != kEmbedding && type_ != kVarchar, "This ype should have type info");

    return LogicalTypeWidth(type_);
}

void DataType::MaxDataType(const DataType &right) {
    if (*this == right) {
        return;
    }

    if (this->type_ == LogicalType::kInvalid) {
        *this = right;
        return;
    }

    if (right.type_ == LogicalType::kInvalid) {
        return;
    }

    if (this->IsNumeric() && right.IsNumeric()) {
        if (this->type_ > right.type_) {
            return;
        } else {
            *this = right;
            return;
        }
    }

    if (this->type_ == LogicalType::kVarchar) {
        return;
    }
    if (right.type_ == LogicalType::kVarchar) {
        *this = right;
        return;
    }

    if (this->type_ == LogicalType::kDateTime and right.type_ == LogicalType::kTimestamp) {
        *this = right;
        return;
    }

    if (this->type_ == LogicalType::kTimestamp and right.type_ == LogicalType::kDateTime) {
        return;
    }

    ParserError(fmt::format("Max type of left: {} and right: {}", this->ToString(), right.ToString()));
}

int32_t DataType::GetSizeInBytes() const {
    int32_t size = sizeof(LogicalType);
    if (this->type_info_ != nullptr) {
        switch (this->type_) {
            case LogicalType::kArray:
                ParserError("Array isn't implemented here.");
                break;
            case LogicalType::kBitmap:
                size += sizeof(int64_t);
                break;
            case LogicalType::kDecimal:
                size += sizeof(int64_t) * 2;
                break;
            case LogicalType::kEmbedding:
                size += sizeof(EmbeddingDataType);
                size += sizeof(int32_t);
                break;
            case LogicalType::kVarchar:
                size += sizeof(int32_t);
                break;
            default:
                ParserError(fmt::format("Unexpected type {} here.", int(this->type_)));
        }
    }
    return size;
}

void DataType::WriteAdv(char *&ptr) const {
    WriteBufAdv<LogicalType>(ptr, this->type_);
    switch (this->type_) {
        case LogicalType::kArray:
            ParserError("Array isn't implemented here.");
            break;
        case LogicalType::kBitmap: {
            int64_t limit = MAX_BITMAP_SIZE_INTERNAL;
            if (this->type_info_ != nullptr) {
                const BitmapInfo *bitmap_info = dynamic_cast<BitmapInfo *>(this->type_info_.get());
                if (bitmap_info != nullptr)
                    limit = bitmap_info->length_limit();
            }
            WriteBufAdv<int64_t>(ptr, limit);
            break;
        }
        case LogicalType::kDecimal: {
            int64_t precision = 0;
            int64_t scale = 0;
            if (this->type_info_ != nullptr) {
                const DecimalInfo *decimal_info = dynamic_cast<DecimalInfo *>(this->type_info_.get());
                if (decimal_info != nullptr) {
                    precision = decimal_info->precision();
                    scale = decimal_info->scale();
                }
            }
            WriteBufAdv<int64_t>(ptr, precision);
            WriteBufAdv<int64_t>(ptr, scale);
            break;
        }
        case LogicalType::kEmbedding: {
            const EmbeddingInfo *embedding_info = dynamic_cast<EmbeddingInfo *>(this->type_info_.get());
            ParserAssert(embedding_info != nullptr, fmt::format("kEmbedding associated type_info is nullptr here."));
            WriteBufAdv<EmbeddingDataType>(ptr, embedding_info->Type());
            WriteBufAdv<int32_t>(ptr, int32_t(embedding_info->Dimension()));
            break;
        }
        case LogicalType::kVarchar: {
            int32_t capacity = MAX_VARCHAR_SIZE_INTERNAL;
            if (this->type_info_ != nullptr) {
                const VarcharInfo *varchar_info = dynamic_cast<VarcharInfo *>(this->type_info_.get());
                if (varchar_info != nullptr)
                    capacity = varchar_info->dimension();
            }
            WriteBufAdv<int32_t>(ptr, capacity);
            break;
        }
        default:
            // There's no type_info for other types
            break;
    }
    return;
}

std::shared_ptr<DataType> DataType::ReadAdv(char *&ptr, int32_t maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    LogicalType type = ReadBufAdv<LogicalType>(ptr);
    std::shared_ptr<TypeInfo> type_info{nullptr};
    switch (type) {
        case LogicalType::kArray:
            ParserError("Array isn't implemented here.");
            break;
        case LogicalType::kBitmap: {
            int64_t limit = ReadBufAdv<int64_t>(ptr);
            type_info = BitmapInfo::Make(limit);
            break;
        }
        case LogicalType::kDecimal: {
            int64_t precision = ReadBufAdv<int64_t>(ptr);
            int64_t scale = ReadBufAdv<int64_t>(ptr);
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
            // There's no type_info for other types
            break;
    }
    maxbytes = ptr_end - ptr;
    ParserAssert(maxbytes >= 0, "ptr goes out of range when reading DataType");
    std::shared_ptr<DataType> data_type = std::make_shared<DataType>(type, type_info);
    return data_type;
}

nlohmann::json DataType::Serialize() {
    nlohmann::json json_res;
    json_res["data_type"] = this->type_;

    if (this->type_info_ != nullptr) {
        json_res["type_info"] = this->type_info_->Serialize();
    }

    return json_res;
}

std::shared_ptr<DataType> DataType::Deserialize(const nlohmann::json &data_type_json) {
    LogicalType logical_type = data_type_json["data_type"];
    std::shared_ptr<TypeInfo> type_info{nullptr};
    if (data_type_json.contains("type_info")) {
        const nlohmann::json &type_info_json = data_type_json["type_info"];
        switch (logical_type) {
            case LogicalType::kArray: {
                ParserError("Array isn't implemented here.");
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
            default:
                // There's no type_info for other types
                break;
        }
    }
    std::shared_ptr<DataType> data_type = std::make_shared<DataType>(logical_type, type_info);
    return data_type;
}

template <>
std::string DataType::TypeToString<BooleanT>() {
    return "Boolean";
}

template <>
std::string DataType::TypeToString<TinyIntT>() {
    return "TinyInt";
}

template <>
std::string DataType::TypeToString<SmallIntT>() {
    return "SmallInt";
}

template <>
std::string DataType::TypeToString<IntegerT>() {
    return "Integer";
}

template <>
std::string DataType::TypeToString<BigIntT>() {
    return "BigInt";
}

template <>
std::string DataType::TypeToString<HugeIntT>() {
    return "HugeInt";
}

template <>
std::string DataType::TypeToString<FloatT>() {
    return "Float";
}

template <>
std::string DataType::TypeToString<DoubleT>() {
    return "Double";
}

template <>
std::string DataType::TypeToString<DecimalT>() {
    return "Decimal";
}

template <>
std::string DataType::TypeToString<VarcharT>() {
    return "Varchar";
}

template <>
std::string DataType::TypeToString<DateT>() {
    return "Date";
}

template <>
std::string DataType::TypeToString<TimeT>() {
    return "Time";
}

template <>
std::string DataType::TypeToString<DateTimeT>() {
    return "DateTime";
}

template <>
std::string DataType::TypeToString<TimestampT>() {
    return "Timestamp";
}

template <>
std::string DataType::TypeToString<IntervalT>() {
    return "Interval";
}

template <>
std::string DataType::TypeToString<ArrayT>() {
    return "Array";
}

// template <> std::string DataType::TypeToString<TupleT>() { return "Tuple"; }
template <>
std::string DataType::TypeToString<PointT>() {
    return "Point";
}

template <>
std::string DataType::TypeToString<LineT>() {
    return "Line";
}

template <>
std::string DataType::TypeToString<LineSegT>() {
    return "LineSegment";
}

template <>
std::string DataType::TypeToString<BoxT>() {
    return "Box";
}

template <>
std::string DataType::TypeToString<PathT>() {
    return "Path";
}

template <>
std::string DataType::TypeToString<PolygonT>() {
    return "Polygon";
}

template <>
std::string DataType::TypeToString<CircleT>() {
    return "Circle";
}

template <>
std::string DataType::TypeToString<BitmapT>() {
    return "Bitmap";
}

template <>
std::string DataType::TypeToString<UuidT>() {
    return "UUID";
}

template <>
std::string DataType::TypeToString<BlobT>() {
    return "Blob";
}

template <>
std::string DataType::TypeToString<EmbeddingT>() {
    return "Embedding";
}

template <>
std::string DataType::TypeToString<RowT>() {
    return "RowID";
}

template <>
std::string DataType::TypeToString<MixedT>() {
    return "Heterogeneous";
}

template <>
BooleanT DataType::StringToValue<BooleanT>(const std::string_view &str) {
    if (str.empty()) {
        return BooleanT{};
    }
    // TODO: should support True/False, maybe others
    std::string str_lower;
    for (char ch : str) {
        str_lower.push_back(std::tolower(ch));
    }
    ParserAssert(str_lower == "true" || str_lower == "false", "Boolean type should be true or false");
    return str_lower == "true";
}

template <>
TinyIntT DataType::StringToValue<TinyIntT>(const std::string_view &str) {
    if (str.empty()) {
        return TinyIntT{};
    }
    TinyIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    ParserAssert(res.ptr == str.data() + str.size(), "Parse TinyInt error"); // TODO: throw error here
    return value;
}

template <>
SmallIntT DataType::StringToValue<SmallIntT>(const std::string_view &str) {
    if (str.empty()) {
        return SmallIntT{};
    }
    SmallIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    ParserAssert(res.ptr == str.data() + str.size(), "Parse SmallInt error");
    return value;
}

template <>
IntegerT DataType::StringToValue<IntegerT>(const std::string_view &str) {
    if (str.empty()) {
        return IntegerT{};
    }
    IntegerT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    ParserAssert(res.ptr == str.data() + str.size(), "Parse Integer error");
    return value;
}

template <>
BigIntT DataType::StringToValue<BigIntT>(const std::string_view &str) {
    if (str.empty()) {
        return BigIntT{};
    }
    BigIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    ParserAssert(res.ptr == str.data() + str.size(), "Parse BigInt error");
    return value;
}

template <>
FloatT DataType::StringToValue<FloatT>(const std::string_view &str) {
    if (str.empty()) {
        return FloatT{};
    }
    FloatT value{};
#if defined(__APPLE__)
    auto ret = std::sscanf(str.data(), "%a", &value);
    ParserAssert(ret == str.size(), "Parse Float error");
#else
    auto res = std::from_chars(str.begin(), str.end(), value);
    ParserAssert(res.ptr == str.data() + str.size(), "Parse Float error");
#endif
    return value;
}

template <>
DoubleT DataType::StringToValue<DoubleT>(const std::string_view &str) {
    if (str.empty()) {
        return DoubleT{};
    }
    DoubleT value{};
#if defined(__APPLE__)
    auto ret = std::sscanf(str.data(), "%la", &value);
    ParserAssert(ret == str.size(), "Parse Double error");
#else
    auto res = std::from_chars(str.begin(), str.end(), value);
    ParserAssert(res.ptr == str.data() + str.size(), "Parse Double error");
#endif
    return value;
}
} // namespace infinity
