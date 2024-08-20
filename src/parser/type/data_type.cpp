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

#include "data_type.h"
#include "info/bitmap_info.h"
#include "serialize.h"
#include "spdlog/fmt/fmt.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"
#include "type/info/sparse_info.h"
#include "type/logical_type.h"
#include "type/type_info.h"
#include <charconv>
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 

namespace infinity {

DataType::DataType(LogicalType logical_type, std::shared_ptr<TypeInfo> type_info_ptr) : type_(logical_type), type_info_(std::move(type_info_ptr)) {
    switch (logical_type) {
        case kBoolean: {
            plain_type_ = false;
            break;
        }
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kFloat16:
        case kBFloat16:
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
//        case kBitmap:
        case kUuid:
        case kEmbedding:
        case kRowID: {
            plain_type_ = true;
            break;
        }
        case kMixed:
        case kVarchar:
        case kSparse:
        case kTensor:
        case kTensorArray:
        case kMultiVector:
        case kArray:
        case kTuple: {
//        case kPath:
//        case kPolygon:
//        case kBlob:
            plain_type_ = false;
            break;
        }
        case kNull:
        case kEmptyArray:
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
        ParserError(fmt::format("Invalid logical data type {}.", LogicalType2Str(type_)));
    }
    if (type_info_.get() != nullptr) {
        return fmt::format("{}({})", LogicalType2Str(type_), type_info_->ToString());
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
        ParserError(fmt::format("Invalid logical data type {}.", LogicalType2Str(type_)));
    }
    switch (type_) {
        case LogicalType::kEmbedding:
        case LogicalType::kSparse: {
            // embedding type should get size from EmbeddingInfo
            return type_info_->Size();
        }
        default: {
            return LogicalTypeWidth(type_);
        }
    }
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
//            case LogicalType::kBitmap:
//                size += sizeof(int64_t);
//                break;
            case LogicalType::kDecimal:
                size += sizeof(int64_t) * 2;
                break;
            case LogicalType::kTensor:
            case LogicalType::kTensorArray:
            case LogicalType::kMultiVector:
            case LogicalType::kEmbedding:
                size += sizeof(EmbeddingDataType);
                size += sizeof(int32_t);
                break;
            case LogicalType::kSparse: {
                size += sizeof(EmbeddingDataType) * 2;
                size += sizeof(int64_t);
                size += sizeof(SparseStoreType);
                break;
            }
            default:
                ParserError(fmt::format("Unexpected type {} here.", LogicalType2Str(this->type_)));
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
//        case LogicalType::kBitmap: {
//            int64_t limit = MAX_BITMAP_SIZE_INTERNAL;
//            if (this->type_info_ != nullptr) {
//                const BitmapInfo *bitmap_info = dynamic_cast<BitmapInfo *>(this->type_info_.get());
//                if (bitmap_info != nullptr)
//                    limit = bitmap_info->length_limit();
//            }
//            WriteBufAdv<int64_t>(ptr, limit);
//            break;
//        }
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
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kMultiVector:
        case LogicalType::kEmbedding: {
            const EmbeddingInfo *embedding_info = dynamic_cast<EmbeddingInfo *>(this->type_info_.get());
            ParserAssert(embedding_info != nullptr, fmt::format("kEmbedding associated type_info is nullptr here."));
            WriteBufAdv<EmbeddingDataType>(ptr, embedding_info->Type());
            WriteBufAdv<int32_t>(ptr, int32_t(embedding_info->Dimension()));
            break;
        }
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<SparseInfo *>(this->type_info().get());
            WriteBufAdv<EmbeddingDataType>(ptr, sparse_info->DataType());
            WriteBufAdv<EmbeddingDataType>(ptr, sparse_info->IndexType());
            WriteBufAdv<int64_t>(ptr, sparse_info->Dimension());
            WriteBufAdv<SparseStoreType>(ptr, sparse_info->StoreType());
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
//        case LogicalType::kBitmap: {
//            int64_t limit = ReadBufAdv<int64_t>(ptr);
//            type_info = BitmapInfo::Make(limit);
//            break;
//        }
        case LogicalType::kDecimal: {
            int64_t precision = ReadBufAdv<int64_t>(ptr);
            int64_t scale = ReadBufAdv<int64_t>(ptr);
            type_info = DecimalInfo::Make(precision, scale);
            break;
        }
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kMultiVector:
        case LogicalType::kEmbedding: {
            EmbeddingDataType embedding_type = ReadBufAdv<EmbeddingDataType>(ptr);
            int32_t dimension = ReadBufAdv<int32_t>(ptr);
            type_info = EmbeddingInfo::Make(EmbeddingDataType(embedding_type), dimension);
            break;
        }
        case LogicalType::kSparse: {
            EmbeddingDataType data_type = ReadBufAdv<EmbeddingDataType>(ptr);
            EmbeddingDataType index_type = ReadBufAdv<EmbeddingDataType>(ptr);
            int64_t dimension = ReadBufAdv<int64_t>(ptr);
            SparseStoreType store_type = ReadBufAdv<SparseStoreType>(ptr);
            auto sparse_info = SparseInfo::Make(data_type, index_type, dimension, store_type);
            type_info = sparse_info;
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
                //            case LogicalType::kBitmap: {
                //                type_info = BitmapInfo::Make(type_info_json["length_limit"]);
                //                break;
                //            }
            case LogicalType::kDecimal: {
                type_info = DecimalInfo::Make(type_info_json["precision"], type_info_json["scale"]);
                break;
            }
            case LogicalType::kTensor:
            case LogicalType::kTensorArray:
            case LogicalType::kMultiVector:
            case LogicalType::kEmbedding: {
                type_info = EmbeddingInfo::Make(type_info_json["embedding_type"], type_info_json["dimension"]);
                break;
            }
            case LogicalType::kSparse: {
                type_info = SparseInfo::Deserialize(type_info_json);
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

std::shared_ptr<DataType> DataType::StringDeserialize(const std::string &data_type_string) {
    const LogicalType logical_type = Str2LogicalType(data_type_string);

    switch (logical_type) {
        case LogicalType::kArray:
        case LogicalType::kDecimal:
        case LogicalType::kEmbedding: {
            return nullptr;
        }
            case LogicalType::kInvalid: {
                ParserError("Invalid data type");
            }
            default: {
                // There's no type_info for other types
                break;
            }
    }
    std::shared_ptr<DataType> data_type = std::make_shared<DataType>(logical_type, nullptr);
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
std::string DataType::TypeToString<Float16T>() {
    return "Float16";
}

template <>
std::string DataType::TypeToString<BFloat16T>() {
    return "BFloat16";
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

//template <>
//std::string DataType::TypeToString<PathT>() {
//    return "Path";
//}
//
//template <>
//std::string DataType::TypeToString<PolygonT>() {
//    return "Polygon";
//}

template <>
std::string DataType::TypeToString<CircleT>() {
    return "Circle";
}

//template <>
//std::string DataType::TypeToString<BitmapT>() {
//    return "Bitmap";
//}

template <>
std::string DataType::TypeToString<UuidT>() {
    return "UUID";
}

//template <>
//std::string DataType::TypeToString<BlobT>() {
//    return "Blob";
//}

template <>
std::string DataType::TypeToString<EmbeddingT>() {
    return "Embedding";
}

template <>
std::string DataType::TypeToString<RowID>() {
    return "RowID";
}

template <>
std::string DataType::TypeToString<MixedT>() {
    return "Heterogeneous";
}

template <>
std::string DataType::TypeToString<TensorT>() {
    return "Tensor";
}

template <>
std::string DataType::TypeToString<TensorArrayT>() {
    return "TensorArray";
}

template <>
std::string DataType::TypeToString<MultiVectorT>() {
    return "MultiVector";
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
uint8_t DataType::StringToValue<uint8_t>(const std::string_view &str) {
    if (str.empty()) {
        return {};
    }
    uint8_t value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    if(res.ptr != str.data() + str.size()) {
        std::string error_message = fmt::format("Error: parse u8 integer: {} to {}", str, value);
        std::cerr << error_message << std::endl;
        ParserError(error_message);
    }
    return value;
}

template <>
TinyIntT DataType::StringToValue<TinyIntT>(const std::string_view &str) {
    if (str.empty()) {
        return TinyIntT{};
    }
    TinyIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    if(res.ptr != str.data() + str.size()) {
        std::string error_message = fmt::format("Error: parse tiny integer: {} to {}", str, value);
        std::cerr << error_message << std::endl;
        ParserError(error_message);
    }
    return value;
}

template <>
SmallIntT DataType::StringToValue<SmallIntT>(const std::string_view &str) {
    if (str.empty()) {
        return SmallIntT{};
    }
    SmallIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    if(res.ptr != str.data() + str.size()) {
        std::string error_message = fmt::format("Error: parse small integer: {} to {}", str, value);
        std::cerr << error_message << std::endl;
        ParserError(error_message);
    }
    return value;
}

template <>
IntegerT DataType::StringToValue<IntegerT>(const std::string_view &str) {
    if (str.empty()) {
        return IntegerT{};
    }
    IntegerT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    if(res.ptr != str.data() + str.size()) {
        std::string error_message = fmt::format("Error: parse integer: {} to {}", str, value);
        std::cerr << error_message << std::endl;
        ParserError(error_message);
    }
    return value;
}

template <>
BigIntT DataType::StringToValue<BigIntT>(const std::string_view &str) {
    if (str.empty()) {
        return BigIntT{};
    }
    BigIntT value{};
    auto res = std::from_chars(str.begin(), str.end(), value);
    if(res.ptr != str.data() + str.size()) {
        std::string error_message = fmt::format("Error: parse big integer: {} to {}", str, value);
        std::cerr << error_message << std::endl;
        ParserError(error_message);
    }
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
    ParserAssert(ret == str.size(), "Error: parse float error");
#else
    // Used in libc++
    try {
        const std::string float_str(str);
        value = std::stof(float_str);
    } catch (const std::exception &e) {
        std::string error_message = fmt::format("Error: parse float: {} to {}", str, value);
        std::cerr << error_message << std::endl;
        ParserError(error_message);
    }
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
    ParserAssert(ret == str.size(), "Error: parse double error");
#else
    try {
        const std::string double_str(str);
        value = std::stod(double_str);
    } catch (const std::exception &e) {
        std::string error_message = fmt::format("Error: parse double: {} to {}", str, value);
        std::cerr << error_message << std::endl;
        ParserError(error_message);
    }
#endif
    return value;
}

template <>
Float16T DataType::StringToValue<Float16T>(const std::string_view &str) {
    FloatT float_value = StringToValue<FloatT>(str);
    return static_cast<Float16T>(float_value);
}

template <>
BFloat16T DataType::StringToValue<BFloat16T>(const std::string_view &str) {
    FloatT float_value = StringToValue<FloatT>(str);
    return static_cast<BFloat16T>(float_value);
}

} // namespace infinity
