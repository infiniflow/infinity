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

module;

#include <charconv>
#include <ranges>
#include <sstream>

module value;

import stl;
import global_resource_usage;
import logger;

import infinity_exception;
import third_party;
import bound_cast_func;
import cast_function;
import column_vector;
import default_values;
import status;
import type_info;

namespace infinity {

namespace {
template <typename T>
void Embedding2JsonInternal(const EmbeddingType &embedding, size_t dimension, nlohmann::json &embedding_json) {
    for (size_t i = 0; i < dimension; ++i) {
        auto data = ((T *)(embedding.ptr))[i];
        if constexpr (std::is_same_v<decltype(data), Float16T> || std::is_same_v<decltype(data), BFloat16T>) {
            embedding_json.push_back(static_cast<f32>(data));
        } else {
            embedding_json.push_back(data);
        }
    }
}

void BitmapEmbedding2JsonInternal(const EmbeddingType &embedding, size_t dimension, nlohmann::json &embedding_json) {
    if (dimension % 8 != 0) {
        Status status = Status::SyntaxError("Binary embedding dimension should be the times of 8.");
        RecoverableError(status);
    }

    auto *array = reinterpret_cast<const u8 *>(embedding.ptr);
    for (size_t i = 0; i < dimension / 8; ++i) {
        const u8 byte = array[i];
        for (size_t j = 0; j < 8; ++j) {
            int8_t elem = (byte & (1 << j)) ? 1 : 0;
            embedding_json.push_back(elem);
        }
    }
}

void Embedding2Json(const EmbeddingType &embedding, EmbeddingDataType type, size_t dimension, nlohmann::json &embedding_json) {
    switch (type) {
        case EmbeddingDataType::kElemBit: {
            BitmapEmbedding2JsonInternal(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            Embedding2JsonInternal<int8_t>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            Embedding2JsonInternal<int16_t>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            Embedding2JsonInternal<int32_t>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            Embedding2JsonInternal<int64_t>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            Embedding2JsonInternal<float>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            Embedding2JsonInternal<double>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            Embedding2JsonInternal<uint8_t>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            Embedding2JsonInternal<Float16T>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            Embedding2JsonInternal<BFloat16T>(embedding, dimension, embedding_json);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            Status status = Status::SyntaxError("Unexpected embedding type");
            RecoverableError(status);
        }
    }
}

void Tensor2Json(Span<char> data_span, const EmbeddingDataType type, const size_t embedding_dimension, nlohmann::json &tensor_json) {
    const size_t data_bytes = data_span.size();
    const size_t basic_embedding_bytes = EmbeddingT::EmbeddingSize(type, embedding_dimension);
    if (data_bytes == 0 or data_bytes % basic_embedding_bytes != 0) {
        UnrecoverableError("Tensor data size mismatch.");
    }
    const size_t embedding_num = data_bytes / basic_embedding_bytes;
    EmbeddingT embedding(nullptr, false);
    for (size_t i = 0; i < embedding_num; ++i) {
        embedding.ptr = data_span.data() + i * basic_embedding_bytes;
        nlohmann::json embedding_json;
        Embedding2Json(embedding, type, embedding_dimension, embedding_json);
        tensor_json.emplace_back(std::move(embedding_json));
    }
}

void TensorArray2Json(const Vector<Span<char>> &tensor_array,
                      const EmbeddingDataType type,
                      const size_t embedding_dimension,
                      nlohmann::json &tensorarray_json) {
    for (const auto tensor_span : tensor_array) {
        nlohmann::json tensor_json;
        Tensor2Json(tensor_span, type, embedding_dimension, tensor_json);
        tensorarray_json.emplace_back(std::move(tensor_json));
    }
}

template <typename IndexT, typename DataType>
void Sparse2JsonInternal1(const SparseValueInfo &sparse_value_info, nlohmann::json &sparse_json) {
    auto [nnz, indice_span, data_span] = sparse_value_info.GetData();
    for (SizeT i = 0; i < nnz; ++i) {
        auto index = reinterpret_cast<IndexT *>(indice_span.data())[i];
        auto data = reinterpret_cast<DataType *>(data_span.data())[i];
        if constexpr (std::is_same_v<decltype(data), Float16T> || std::is_same_v<decltype(data), BFloat16T>) {
            sparse_json[std::to_string(index)] = static_cast<f32>(data);
        } else {
            sparse_json[std::to_string(index)] = data;
        }
    }
}

template <typename IndexT>
void Sparse2JsonInternal(const SparseValueInfo &sparse_value_info, const SparseInfo *sparse_info, nlohmann::json &sparse_json) {
    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemInt8: {
            Sparse2JsonInternal1<IndexT, i8>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            Sparse2JsonInternal1<IndexT, i16>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            Sparse2JsonInternal1<IndexT, i32>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            Sparse2JsonInternal1<IndexT, i64>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            Sparse2JsonInternal1<IndexT, f32>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            Sparse2JsonInternal1<IndexT, f64>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            Sparse2JsonInternal1<IndexT, u8>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            Sparse2JsonInternal1<IndexT, Float16T>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            Sparse2JsonInternal1<IndexT, BFloat16T>(sparse_value_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemBit:
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Not implemented.");
        }
    }
}

void Sparse2Json(const SparseValueInfo &sparse_value_info, const SparseInfo *sparse_info, nlohmann::json &sparse_json) {
    switch (sparse_info->IndexType()) {
        case EmbeddingDataType::kElemInt8: {
            Sparse2JsonInternal<i8>(sparse_value_info, sparse_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            Sparse2JsonInternal<i16>(sparse_value_info, sparse_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            Sparse2JsonInternal<i32>(sparse_value_info, sparse_info, sparse_json);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            Sparse2JsonInternal<i64>(sparse_value_info, sparse_info, sparse_json);
            break;
        }
        default: {
            UnrecoverableError("Not implemented.");
        }
    }
}

void Embedding2Arrow(const EmbeddingType &embedding, EmbeddingDataType type, size_t dimension, arrow::ArrayBuilder *value_builder) {
    switch (type) {
        case EmbeddingDataType::kElemBit: {
            if (dimension % 8 != 0 || dimension == 0) {
                Status status = Status::SyntaxError("Bit embedding dimension should be the times of 8.");
                LOG_ERROR(status.message());
                RecoverableError(std::move(status));
            }
            auto builder = dynamic_cast<arrow::BooleanBuilder *>(value_builder);
            const size_t byte_cnt = dimension / 8;
            for (size_t i = 0; i < byte_cnt; ++i) {
                const u8 byte = reinterpret_cast<const u8 *>(embedding.ptr)[i];
                for (size_t j = 0; j < 8; ++j) {
                    const bool bit = (byte & (1u << j)) ? true : false;
                    auto status = builder->Append(bit);
                }
            }
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            auto builder = dynamic_cast<arrow::Int8Builder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                auto status = builder->Append(((int8_t *)(embedding.ptr))[i]);
            }
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            auto builder = dynamic_cast<arrow::Int16Builder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                auto status = builder->Append(((int16_t *)(embedding.ptr))[i]);
            }
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            auto builder = dynamic_cast<arrow::Int32Builder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                auto status = builder->Append(((int32_t *)(embedding.ptr))[i]);
            }
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            auto builder = dynamic_cast<arrow::Int64Builder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                auto status = builder->Append(((int64_t *)(embedding.ptr))[i]);
            }
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            auto builder = dynamic_cast<arrow::FloatBuilder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                auto status = builder->Append(((float *)(embedding.ptr))[i]);
            }
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            auto builder = dynamic_cast<arrow::DoubleBuilder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                auto status = builder->Append(((double *)(embedding.ptr))[i]);
            }
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            auto builder = dynamic_cast<arrow::UInt8Builder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                auto status = builder->Append(((u8 *)(embedding.ptr))[i]);
            }
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            auto builder = dynamic_cast<arrow::HalfFloatBuilder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                const auto data = ((Float16T *)(embedding.ptr))[i];
                auto status = builder->Append(data.raw);
            }
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            auto builder = dynamic_cast<arrow::FloatBuilder *>(value_builder);
            for (size_t i = 0; i < dimension; ++i) {
                const auto data = ((BFloat16T *)(embedding.ptr))[i];
                auto status = builder->Append(static_cast<float>(data));
            }
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            Status status = Status::SyntaxError("Unexpected embedding type");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
}

void Tensor2Arrow(Span<char> tensor, const EmbeddingInfo *embedding_info, arrow::ArrayBuilder *value_builder) {
    const size_t data_bytes = tensor.size();
    const size_t basic_embedding_bytes = embedding_info->Size();
    if (data_bytes == 0 or data_bytes % basic_embedding_bytes != 0) {
        UnrecoverableError("Tensor data size mismatch.");
    }
    const auto embedding_num = data_bytes / basic_embedding_bytes;
    const auto type = embedding_info->Type();
    const auto dimension = embedding_info->Dimension();
    auto *tensor_builder = dynamic_cast<arrow::FixedSizeListBuilder *>(value_builder);
    EmbeddingT embedding(nullptr, false);
    for (SizeT i = 0; i < embedding_num; i++) {
        embedding.ptr = tensor.data() + i * basic_embedding_bytes;
        auto status = tensor_builder->Append();
        Embedding2Arrow(embedding, type, dimension, tensor_builder->value_builder());
    }
}

void TensorArray2Arrow(const Vector<Span<char>> &tensor_array, const EmbeddingInfo *embedding_info, arrow::ArrayBuilder *value_builder) {
    auto *tensor_array_builder = dynamic_cast<arrow::ListBuilder *>(value_builder);
    for (const auto tensor : tensor_array) {
        auto status = tensor_array_builder->Append();
        Tensor2Arrow(tensor, embedding_info, tensor_array_builder->value_builder());
    }
}

} // namespace

EmbeddingValueInfo::EmbeddingValueInfo(const Vector<Pair<ptr_t, SizeT>> &ptr_bytes) : ExtraValueInfo(ExtraValueInfoType::EMBEDDING_VALUE_INFO) {
    len_ = 0;
    for (const auto &[ptr, bytes] : ptr_bytes) {
        len_ += bytes;
    }
    data_ = MakeUnique<char[]>(len_);
    SizeT offset = 0;
    for (const auto &[ptr, bytes] : ptr_bytes) {
        std::memcpy(data_.get() + offset, ptr, bytes);
        offset += bytes;
    }
}

// Value maker
Value Value::MakeValue(DataType type) {
    Value value(type.type(), type.type_info());
    return value;
}

Value Value::MakeNull() {
    Value value(LogicalType::kNull);
    return value;
}

Value Value::MakeEmptyArray() {
    Value value(LogicalType::kEmptyArray);
    return value;
}

Value Value::MakeInvalid() {
    Value value(LogicalType::kInvalid);
    return value;
}

Value Value::MakeBool(BooleanT input) {
    Value value(LogicalType::kBoolean);
    value.value_.boolean = input;
    return value;
}

Value Value::MakeTinyInt(TinyIntT input) {
    Value value(LogicalType::kTinyInt);
    value.value_.tiny_int = input;
    return value;
}

Value Value::MakeSmallInt(SmallIntT input) {
    Value value(LogicalType::kSmallInt);
    value.value_.small_int = input;
    return value;
}

Value Value::MakeInt(IntegerT input) {
    Value value(LogicalType::kInteger);
    value.value_.integer = input;
    return value;
}

Value Value::MakeBigInt(BigIntT input) {
    Value value(LogicalType::kBigInt);
    value.value_.big_int = input;
    return value;
}

Value Value::MakeHugeInt(HugeIntT input) {
    Value value(LogicalType::kHugeInt);
    value.value_.huge_int = input;
    return value;
}

Value Value::MakeFloat(FloatT input) {
    Value value(LogicalType::kFloat);
    value.value_.float32 = input;
    return value;
}

Value Value::MakeDouble(DoubleT input) {
    Value value(LogicalType::kDouble);
    value.value_.float64 = input;
    return value;
}

Value Value::MakeFloat16(Float16T input) {
    Value value(LogicalType::kFloat16);
    value.value_.float16 = input;
    return value;
}

Value Value::MakeBFloat16(BFloat16T input) {
    Value value(LogicalType::kBFloat16);
    value.value_.bfloat16 = input;
    return value;
}

Value Value::MakeDecimal(DecimalT input, SharedPtr<TypeInfo> type_info_ptr) {
    Value value(LogicalType::kDecimal, type_info_ptr);
    value.value_.decimal = input;
    return value;
}

Value Value::MakeDate(DateT input) {
    Value value(LogicalType::kDate);
    value.value_.date = input;
    return value;
}

Value Value::MakeTime(TimeT input) {
    Value value(LogicalType::kTime);
    value.value_.time = input;
    return value;
}

Value Value::MakeDateTime(DateTimeT input) {
    Value value(LogicalType::kDateTime);
    value.value_.datetime = input;
    return value;
}

Value Value::MakeTimestamp(TimestampT input) {
    Value value(LogicalType::kTimestamp);
    value.value_.timestamp = input;
    return value;
}

Value Value::MakeInterval(IntervalT input) {
    Value value(LogicalType::kInterval);
    value.value_.interval = input;
    return value;
}

Value Value::MakePoint(PointT input) {
    Value value(LogicalType::kPoint);
    value.value_.point = input;
    return value;
}

Value Value::MakeLine(LineT input) {
    Value value(LogicalType::kLine);
    value.value_.line = input;
    return value;
}

Value Value::MakeLineSegment(LineSegT input) {
    Value value(LogicalType::kLineSeg);
    value.value_.line_segment = input;
    return value;
}

Value Value::MakeBox(BoxT input) {
    Value value(LogicalType::kBox);
    value.value_.box = input;
    return value;
}

Value Value::MakeCircle(CircleT input) {
    Value value(LogicalType::kCircle);
    value.value_.circle = input;
    return value;
}

Value Value::MakeUuid(UuidT input) {
    Value value(LogicalType::kUuid);
    value.value_.uuid = std::move(input);
    return value;
}

Value Value::MakeRow(RowID input) {
    Value value(LogicalType::kRowID);
    value.value_.row = input;
    return value;
}

// Value Value::MakeVarchar(const String &str) {
//     Value value(LogicalType::kVarchar);
//     value.value_info_ = MakeShared<StringValueInfo>(str);
//     return value;
// }

Value Value::MakeVarchar(const std::string_view str_view) {
    Value value(LogicalType::kVarchar);
    value.value_info_ = MakeShared<StringValueInfo>(str_view);
    return value;
}

Value Value::MakeVarchar(const char *ptr, SizeT len) {
    String tmp_str(ptr, len);
    Value value(LogicalType::kVarchar);
    value.value_info_ = MakeShared<StringValueInfo>(String(ptr, len));
    return value;
}

Value Value::MakeVarchar(const VarcharT &input) {
    Value value(LogicalType::kVarchar);
    if (input.IsInlined()) {
        String tmp_str(input.short_.data_, input.length_);
        value.value_info_ = MakeShared<StringValueInfo>(std::move(tmp_str));
    } else {
        String error_message = "Value::MakeVarchar(VectorVarchar) is unsupported!";
        UnrecoverableError(error_message);
    }
    return value;
}

Value Value::MakeEmbedding(ptr_t ptr, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeEmbedding(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(type_info_ptr.get());
    const SizeT len = embedding_info->Size();
    Value value(LogicalType::kEmbedding, std::move(type_info_ptr));
    value.value_info_ = MakeShared<EmbeddingValueInfo>(ptr, len);
    return value;
}

Value Value::MakeMultiVector(const_ptr_t ptr, SizeT bytes, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeMultiVector(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(type_info_ptr.get());
    if (const SizeT len = embedding_info->Size(); len == 0 or bytes % len != 0) {
        auto status = Status::SyntaxError(fmt::format("Value::MakeMultiVector(bytes={}) is not a multiple of embedding byte size={}", bytes, len));
        RecoverableError(std::move(status));
    }
    Value value(LogicalType::kMultiVector, std::move(type_info_ptr));
    value.value_info_ = EmbeddingValueInfo::MakeMultiVectorValueInfo(ptr, bytes);
    return value;
}

Value Value::MakeMultiVector(const Vector<Pair<ptr_t, SizeT>> &ptr_bytes, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeMultiVector(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(type_info_ptr.get());
    const SizeT len = embedding_info->Size();
    for (const auto [_, bytes] : ptr_bytes) {
        if (bytes == 0 || bytes % len != 0) {
            auto status = Status::SyntaxError(fmt::format("Value::MakeMultiVector(bytes={}) is not a multiple of embedding byte size={}", bytes, len));
            RecoverableError(std::move(status));
        }
    }
    Value value(LogicalType::kMultiVector, std::move(type_info_ptr));
    value.value_info_ = EmbeddingValueInfo::MakeMultiVectorValueInfo(ptr_bytes);
    return value;
}

Value Value::MakeTensor(const_ptr_t ptr, SizeT bytes, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeTensor(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(type_info_ptr.get());
    if (const SizeT len = embedding_info->Size(); len == 0 or bytes % len != 0) {
        auto status = Status::SyntaxError(fmt::format("Value::MakeTensor(bytes={}) is not a multiple of embedding byte size={}", bytes, len));
        RecoverableError(std::move(status));
    }
    Value value(LogicalType::kTensor, std::move(type_info_ptr));
    value.value_info_ = EmbeddingValueInfo::MakeTensorValueInfo(ptr, bytes);
    return value;
}

Value Value::MakeTensor(const Vector<Pair<ptr_t, SizeT>> &ptr_bytes, SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeTensor(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(type_info_ptr.get());
    const SizeT len = embedding_info->Size();
    for (const auto [_, bytes] : ptr_bytes) {
        if (bytes == 0 || bytes % len != 0) {
            auto status = Status::SyntaxError(fmt::format("Value::MakeTensor(bytes={}) is not a multiple of embedding byte size={}", bytes, len));
            RecoverableError(std::move(status));
        }
    }
    Value value(LogicalType::kTensor, std::move(type_info_ptr));
    value.value_info_ = EmbeddingValueInfo::MakeTensorValueInfo(ptr_bytes);
    return value;
}

Value Value::MakeTensorArray(SharedPtr<TypeInfo> type_info_ptr) {
    if (type_info_ptr->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Value::MakeTensorArray(type_info_ptr={}) is not unsupported!", type_info_ptr->ToString());
        UnrecoverableError(error_message);
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(type_info_ptr.get());
    if (const SizeT len = embedding_info->Size(); len == 0) {
        auto status = Status::SyntaxError(fmt::format("Value::MakeTensorArray(unit embedding bytes = {}) is invalid", len));
        RecoverableError(std::move(status));
    }
    Value value(LogicalType::kTensorArray, std::move(type_info_ptr));
    value.value_info_ = MakeShared<TensorArrayValueInfo>();
    return value;
}

Value Value::MakeSparse(const char *raw_data_ptr, const char *raw_idx_ptr, SizeT nnz, const SharedPtr<TypeInfo> type_info) {
    const auto *sparse_info = static_cast<const SparseInfo *>(type_info.get());

    SizeT raw_indice_len = sparse_info->IndiceSize(nnz);
    SizeT raw_data_len = sparse_info->DataSize(nnz);
    Value value(LogicalType::kSparse, type_info);
    value.value_info_ = MakeShared<SparseValueInfo>(nnz, raw_idx_ptr, raw_indice_len, raw_data_ptr, raw_data_len);
    return value;
}

Value Value::MakeSparse(SizeT nnz, UniquePtr<char[]> indice_ptr, UniquePtr<char[]> data_ptr, const SharedPtr<TypeInfo> type_info) {
    Value value(LogicalType::kSparse, type_info);
    const auto *sparse_info = static_cast<const SparseInfo *>(type_info.get());
    SizeT indice_len = sparse_info->IndiceSize(nnz);
    SizeT data_len = sparse_info->DataSize(nnz);
    value.value_info_ = MakeShared<SparseValueInfo>(nnz, std::move(indice_ptr), indice_len, std::move(data_ptr), data_len);
    return value;
}

void Value::AppendToTensorArray(const_ptr_t ptr, SizeT bytes) {
    if (type_.type() != LogicalType::kTensorArray) {
        String error_message = fmt::format("Value::AppendToTensorArray() is not supported for type {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(type_.type_info().get());
    if (const SizeT len = embedding_info->Size(); len == 0 or bytes % len != 0) {
        auto status =
            Status::SyntaxError(fmt::format("Value::AppendToTensorArray(bytes={}) is not a multiple of embedding byte size={}", bytes, len));
        RecoverableError(std::move(status));
    }
    auto &tensor_array_info = value_info_->Get<TensorArrayValueInfo>();
    tensor_array_info.AppendTensor(ptr, bytes);
}

void Value::AppendToTensorArray(const Vector<Pair<ptr_t, SizeT>> &ptr_bytes) {
    if (type_.type() != LogicalType::kTensorArray) {
        String error_message = fmt::format("Value::AppendToTensorArray() is not supported for type {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    auto &tensor_array_info = value_info_->Get<TensorArrayValueInfo>();
    tensor_array_info.AppendTensor(ptr_bytes);
}

// Value getter
template <>
BooleanT Value::GetValue() const {
    if (type_.type() != LogicalType::kBoolean) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.boolean;
}

template <>
TinyIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kTinyInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.tiny_int;
}

template <>
SmallIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kSmallInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.small_int;
}

template <>
IntegerT Value::GetValue() const {
    if (type_.type() != LogicalType::kInteger) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.integer;
}

template <>
BigIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kBigInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.big_int;
}

template <>
HugeIntT Value::GetValue() const {
    if (type_.type() != LogicalType::kHugeInt) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.huge_int;
}

template <>
FloatT Value::GetValue() const {
    if (type_.type() != LogicalType::kFloat) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.float32;
}

template <>
DoubleT Value::GetValue() const {
    if (type_.type() != LogicalType::kDouble) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.float64;
}

template <>
Float16T Value::GetValue() const {
    if (type_.type() != LogicalType::kFloat16) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.float16;
}

template <>
BFloat16T Value::GetValue() const {
    if (type_.type() != LogicalType::kBFloat16) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.bfloat16;
}

template <>
DecimalT Value::GetValue() const {
    if (type_.type() != LogicalType::kDecimal) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.decimal;
}

template <>
DateT Value::GetValue() const {
    if (type_.type() != LogicalType::kDate) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.date;
}

template <>
TimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kTime) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.time;
}

template <>
DateTimeT Value::GetValue() const {
    if (type_.type() != LogicalType::kDateTime) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.datetime;
}

template <>
TimestampT Value::GetValue() const {
    if (type_.type() != LogicalType::kTimestamp) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.timestamp;
}

template <>
IntervalT Value::GetValue() const {
    if (type_.type() != LogicalType::kInterval) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.interval;
}

template <>
PointT Value::GetValue() const {
    if (type_.type() != LogicalType::kPoint) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.point;
}

template <>
LineT Value::GetValue() const {
    if (type_.type() != LogicalType::kLine) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.line;
}

template <>
LineSegT Value::GetValue() const {
    if (type_.type() != LogicalType::kLineSeg) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.line_segment;
}

template <>
BoxT Value::GetValue() const {
    if (type_.type() != LogicalType::kBox) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.box;
}

template <>
CircleT Value::GetValue() const {
    if (type_.type() != LogicalType::kCircle) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.circle;
}

template <>
UuidT Value::GetValue() const {
    if (type_.type() != LogicalType::kUuid) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.uuid;
}

template <>
RowID Value::GetValue() const {
    if (type_.type() != LogicalType::kRowID) {
        String error_message = fmt::format("Not matched type: {}", type_.ToString());
        UnrecoverableError(error_message);
    }
    return value_.row;
}

Value::~Value() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("Value");
#endif
}

Value::Value(const DataType &data_type) : type_(data_type) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
}

Value::Value(LogicalType type, SharedPtr<TypeInfo> typeinfo_ptr) : type_(type, std::move(typeinfo_ptr)) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
}

Value::Value(const Value &other) : type_(other.type_) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
    CopyUnionValue(other);
}

Value::Value(Value &&other) noexcept : type_(other.type_) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Value");
#endif
    MoveUnionValue(std::forward<Value>(other));
}

Value &Value::operator=(const Value &other) {
    if (this == &other)
        return *this;
    this->Reset();
    CopyUnionValue(other);
    return *this;
}

Value &Value::operator=(Value &&other) noexcept {
    this->Reset();
    MoveUnionValue(std::forward<Value>(other));
    return *this;
}

bool Value::operator==(const Value &other) const {
    if (this->type_ != other.type_)
        return false;
    if (this->type_.type() == LogicalType::kInvalid)
        return true;
    switch (this->type_.type()) {
        case LogicalType::kBoolean: {
            return value_.boolean == other.value_.boolean;
        }
        case LogicalType::kTinyInt: {
            return value_.tiny_int == other.value_.tiny_int;
        }
        case LogicalType::kSmallInt: {
            return value_.small_int == other.value_.small_int;
        }
        case LogicalType::kInteger: {
            return value_.integer == other.value_.integer;
        }
        case LogicalType::kBigInt: {
            return value_.big_int == other.value_.big_int;
        }
        case LogicalType::kHugeInt: {
            return value_.huge_int == other.value_.huge_int;
        }
        case LogicalType::kFloat: {
            return value_.float32 == other.value_.float32;
        }
        case LogicalType::kDouble: {
            return value_.float64 == other.value_.float64;
        }
        case LogicalType::kFloat16: {
            return value_.float16 == other.value_.float16;
        }
        case LogicalType::kBFloat16: {
            return value_.bfloat16 == other.value_.bfloat16;
        }
        case LogicalType::kDecimal: {
            return value_.decimal == other.value_.decimal;
        }
        case LogicalType::kDate: {
            return value_.date == other.value_.date;
        }
        case LogicalType::kTime: {
            return value_.time == other.value_.time;
        }
        case LogicalType::kDateTime: {
            return value_.datetime == other.value_.datetime;
        }
        case LogicalType::kTimestamp: {
            return value_.timestamp == other.value_.timestamp;
        }
        case LogicalType::kPoint: {
            return value_.point == other.value_.point;
        }
        case LogicalType::kLine: {
            return value_.line == other.value_.line;
        }
        case LogicalType::kLineSeg: {
            return value_.line_segment == other.value_.line_segment;
        }
        case LogicalType::kBox: {
            return value_.box == other.value_.box;
        }
        case LogicalType::kCircle: {
            return value_.circle == other.value_.circle;
        }
        case LogicalType::kUuid: {
            return value_.uuid == other.value_.uuid;
        }
        case LogicalType::kRowID: {
            return value_.row == other.value_.row;
        }
        case LogicalType::kEmptyArray:
        case LogicalType::kNull: {
            return true;
        }
        case LogicalType::kVarchar: {
            const String &s1 = this->value_info_->Get<StringValueInfo>().GetString();
            const String &s2 = other.value_info_->Get<StringValueInfo>().GetString();
            return s1 == s2;
        }
        case LogicalType::kEmbedding:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor: {
            const Span<char> data1 = this->GetEmbedding();
            const Span<char> data2 = other.GetEmbedding();
            return std::ranges::equal(data1, data2);
        }
        case LogicalType::kTensorArray: {
            const auto &tensor_array1 = this->GetTensorArray();
            const auto &tensor_array2 = other.GetTensorArray();
            return std::ranges::equal(tensor_array1, tensor_array2, [](const auto &t1, const auto &t2) {
                return std::ranges::equal(t1->GetData(), t2->GetData());
            });
        }
        case LogicalType::kSparse: {
            const auto &sparse1 = GetSparse();
            const auto &sparse2 = other.GetSparse();
            const auto &[nnz1, raw_idx1, raw_data1] = sparse1;
            const auto &[nnz2, raw_idx2, raw_data2] = sparse2;
            return nnz1 == nnz2 && std::ranges::equal(raw_idx1, raw_idx2) && std::ranges::equal(raw_data1, raw_data2);
        }
        case LogicalType::kInterval:
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kMixed:
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            String error_message = "Unhandled cases.";
            UnrecoverableError(error_message);
            return false;
        }
    }
    return true;
}

void Value::CopyUnionValue(const Value &other) {
    this->type_ = other.type_;
    switch (type_.type()) {
        case LogicalType::kBoolean: {
            value_.boolean = other.value_.boolean;
            break;
        }
        case LogicalType::kTinyInt: {
            value_.tiny_int = other.value_.tiny_int;
            break;
        }
        case LogicalType::kSmallInt: {
            value_.small_int = other.value_.small_int;
            break;
        }
        case LogicalType::kInteger: {
            value_.integer = other.value_.integer;
            break;
        }
        case LogicalType::kBigInt: {
            value_.big_int = other.value_.big_int;
            break;
        }
        case LogicalType::kHugeInt: {
            value_.huge_int = other.value_.huge_int;
            break;
        }
        case LogicalType::kFloat: {
            value_.float32 = other.value_.float32;
            break;
        }
        case LogicalType::kDouble: {
            value_.float64 = other.value_.float64;
            break;
        }
        case LogicalType::kFloat16: {
            value_.float16 = other.value_.float16;
            break;
        }
        case LogicalType::kBFloat16: {
            value_.bfloat16 = other.value_.bfloat16;
            break;
        }
        case LogicalType::kDecimal: {
            value_.decimal = other.value_.decimal;
            break;
        }
        case LogicalType::kDate: {
            value_.date = other.value_.date;
            break;
        }
        case LogicalType::kTime: {
            value_.time = other.value_.time;
            break;
        }
        case LogicalType::kDateTime: {
            value_.datetime = other.value_.datetime;
            break;
        }
        case LogicalType::kTimestamp: {
            value_.timestamp = other.value_.timestamp;
            break;
        }
        case LogicalType::kInterval: {
            value_.interval = other.value_.interval;
            break;
        }
        case LogicalType::kPoint: {
            value_.point = other.value_.point;
            break;
        }
        case LogicalType::kLine: {
            value_.line = other.value_.line;
            break;
        }
        case LogicalType::kLineSeg: {
            value_.line_segment = other.value_.line_segment;
            break;
        }
        case LogicalType::kBox: {
            value_.box = other.value_.box;
            break;
        }
        case LogicalType::kCircle: {
            value_.circle = other.value_.circle;
            break;
        }
        case LogicalType::kUuid: {
            value_.uuid = other.value_.uuid;
            break;
        }
        case LogicalType::kRowID: {
            value_.row = other.value_.row;
            break;
        }
        case LogicalType::kEmptyArray:
        case LogicalType::kNull: {
            // No value for null value.
            break;
        }
        case LogicalType::kVarchar:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kMultiVector:
        case LogicalType::kEmbedding:
        case LogicalType::kSparse: {
            this->value_info_ = other.value_info_;
            break;
        }
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kMixed:
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            String error_message = "Unhandled cases.";
            UnrecoverableError(error_message);
            break;
        }
    }
}

void Value::MoveUnionValue(Value &&other) noexcept {
    this->type_ = std::move(other.type_);
    switch (this->type_.type()) {
        case LogicalType::kBoolean: {
            this->value_.boolean = other.value_.boolean;
            break;
        }
        case LogicalType::kTinyInt: {
            this->value_.tiny_int = other.value_.tiny_int;
            break;
        }
        case LogicalType::kSmallInt: {
            this->value_.small_int = other.value_.small_int;
            break;
        }
        case LogicalType::kInteger: {
            this->value_.integer = other.value_.integer;
            break;
        }
        case LogicalType::kBigInt: {
            this->value_.big_int = other.value_.big_int;
            break;
        }
        case LogicalType::kHugeInt: {
            this->value_.huge_int = other.value_.huge_int;
            break;
        }
        case LogicalType::kFloat: {
            this->value_.float32 = other.value_.float32;
            break;
        }
        case LogicalType::kDouble: {
            this->value_.float64 = other.value_.float64;
            break;
        }
        case LogicalType::kFloat16: {
            this->value_.float16 = other.value_.float16;
            break;
        }
        case LogicalType::kBFloat16: {
            this->value_.bfloat16 = other.value_.bfloat16;
            break;
        }
        case LogicalType::kDecimal: {
            this->value_.decimal = other.value_.decimal;
            break;
        }
        case LogicalType::kDate: {
            this->value_.date = other.value_.date;
            break;
        }
        case LogicalType::kTime: {
            this->value_.time = other.value_.time;
            break;
        }
        case LogicalType::kDateTime: {
            this->value_.datetime = other.value_.datetime;
            break;
        }
        case LogicalType::kTimestamp: {
            this->value_.timestamp = other.value_.timestamp;
            break;
        }
        case LogicalType::kInterval: {
            this->value_.interval = other.value_.interval;
            break;
        }
        case LogicalType::kPoint: {
            this->value_.point = other.value_.point;
            break;
        }
        case LogicalType::kLine: {
            this->value_.line = other.value_.line;
            break;
        }
        case LogicalType::kLineSeg: {
            this->value_.line_segment = other.value_.line_segment;
            break;
        }
        case LogicalType::kBox: {
            this->value_.box = other.value_.box;
            break;
        }
        case LogicalType::kCircle: {
            this->value_.circle = other.value_.circle;
            break;
        }
        case LogicalType::kUuid: {
            this->value_.uuid = std::move(other.value_.uuid);
            break;
        }
        case LogicalType::kRowID: {
            value_.row = other.value_.row;
            break;
        }
        case LogicalType::kEmptyArray:
        case LogicalType::kNull: {
            // No value for null type
            break;
        }
        case LogicalType::kVarchar:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kMultiVector:
        case LogicalType::kEmbedding:
        case LogicalType::kSparse: {
            this->value_info_ = std::move(other.value_info_);
            break;
        }
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kMixed:
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            String error_message = "Unhandled cases.";
            UnrecoverableError(error_message);
            break;
        }
    }
}

void Value::Reset() {
    this->value_info_.reset();
    this->type_.Reset();
}

String Value::ToString() const {
    switch (type_.type()) {
        case LogicalType::kBoolean: {
            return value_.boolean ? "true" : "false";
        }
        case LogicalType::kTinyInt: {
            return std::to_string(value_.tiny_int);
        }
        case LogicalType::kSmallInt: {
            return std::to_string(value_.small_int);
        }
        case LogicalType::kInteger: {
            return std::to_string(value_.integer);
        }
        case LogicalType::kBigInt: {
            return std::to_string(value_.big_int);
        }
        case LogicalType::kHugeInt: {
            return value_.huge_int.ToString();
        }
        case LogicalType::kFloat: {
            char buffer[20];
            auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value_.float32);
            if (ec != std::errc()) {
                String error_message = "Float to string conversion failed.";
                UnrecoverableError(error_message);
            }
            String result(buffer, ptr);
            return result;
        }
        case LogicalType::kDouble: {
            char buffer[30];
            auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value_.float64);
            if (ec != std::errc()) {
                String error_message = "Double to string conversion failed.";
                UnrecoverableError(error_message);
            }
            String result(buffer, ptr);
            return result;
        }
        case LogicalType::kFloat16: {
            char buffer[20];
            auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), static_cast<float>(value_.float16));
            if (ec != std::errc()) {
                String error_message = "Float to string conversion failed.";
                UnrecoverableError(error_message);
            }
            String result(buffer, ptr);
            return result;
        }
        case LogicalType::kBFloat16: {
            char buffer[20];
            auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), static_cast<float>(value_.bfloat16));
            if (ec != std::errc()) {
                String error_message = "Float to string conversion failed.";
                UnrecoverableError(error_message);
            }
            String result(buffer, ptr);
            return result;
        }
        case LogicalType::kDate: {
            return value_.date.ToString();
        }
        case LogicalType::kTime: {
            return value_.time.ToString();
        }
        case LogicalType::kDateTime: {
            return value_.datetime.ToString();
        }
        case LogicalType::kTimestamp: {
            return value_.timestamp.ToString();
        }
        case LogicalType::kInterval: {
            return value_.interval.ToString();
        }
        case LogicalType::kRowID: {
            return value_.row.ToString();
        }
        case LogicalType::kVarchar: {
            return value_info_->Get<StringValueInfo>().GetString();
        }
        case LogicalType::kEmbedding: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_.type_info().get());
            Span<char> data_span = this->GetEmbedding();
            if (data_span.size() != embedding_info->Size()) {
                String error_message = "Embedding data size mismatch.";
                UnrecoverableError(error_message);
            }
            const EmbeddingT embedding(data_span.data(), false);
            return EmbeddingT::Embedding2String(embedding, embedding_info->Type(), embedding_info->Dimension());
        }
        case LogicalType::kMultiVector:
        case LogicalType::kTensor: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_.type_info().get());
            Span<char> data_span = this->GetEmbedding();
            SizeT data_bytes = data_span.size();
            const auto basic_embedding_bytes = embedding_info->Size();
            if (data_bytes == 0 or data_bytes % basic_embedding_bytes != 0) {
                UnrecoverableError("Tensor data size mismatch.");
            }
            const auto embedding_num = data_bytes / basic_embedding_bytes;
            return TensorT::Tensor2String(data_span.data(), embedding_info->Type(), embedding_info->Dimension(), embedding_num);
        }
        case LogicalType::kTensorArray: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_.type_info().get());
            const auto &embedding_value_infos = this->GetTensorArray();
            const SizeT basic_embedding_bytes = embedding_info->Size();
            std::ostringstream oss;
            oss << '[';
            for (SizeT i = 0; i < embedding_value_infos.size(); ++i) {
                const auto &embedding_value_info = embedding_value_infos[i];
                Span<char> data_span = embedding_value_info->GetData();
                const SizeT data_bytes = data_span.size();
                if (data_bytes == 0 or data_bytes % basic_embedding_bytes != 0) {
                    UnrecoverableError("TensorArray data size mismatch.");
                }
                const auto embedding_num = data_bytes / basic_embedding_bytes;
                oss << TensorT::Tensor2String(data_span.data(), embedding_info->Type(), embedding_info->Dimension(), embedding_num);
                if (i != embedding_value_infos.size() - 1) {
                    oss << ',';
                }
            }
            oss << ']';
            return std::move(oss).str();
        }
        case LogicalType::kSparse: {
            auto *sparse_info = static_cast<SparseInfo *>(type_.type_info().get());
            auto [nnz, indice_span, data_span] = this->GetSparse();
            return SparseT::Sparse2String(data_span.data(), indice_span.data(), sparse_info->DataType(), sparse_info->IndexType(), nnz);
        }
        case LogicalType::kEmptyArray: {
            return "[]";
        }
        case LogicalType::kDecimal:
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            String error_message = fmt::format("Value::ToString() not implemented for type {}", type_.ToString());
            UnrecoverableError(error_message);
            return {};
        }
    }
    return {};
}

void Value::AppendToJson(const String &name, nlohmann::json &json) {
    switch (type_.type()) {
        case LogicalType::kBoolean: {
            json[name] = value_.boolean;
            return;
        }
        case LogicalType::kTinyInt: {
            json[name] = value_.tiny_int;
            return;
        }
        case LogicalType::kSmallInt: {
            json[name] = value_.small_int;
            return;
        }
        case LogicalType::kInteger: {
            json[name] = value_.integer;
            return;
        }
        case LogicalType::kBigInt: {
            json[name] = value_.big_int;
            return;
        }
        case LogicalType::kFloat: {
            json[name] = value_.float32;
            return;
        }
        case LogicalType::kDouble: {
            json[name] = value_.float64;
            return;
        }
        case LogicalType::kFloat16: {
            json[name] = static_cast<float>(value_.float16);
            return;
        }
        case LogicalType::kBFloat16: {
            json[name] = static_cast<float>(value_.bfloat16);
            return;
        }
        case LogicalType::kDate: {
            json[name] = value_.date.ToString();
            return;
        }
        case LogicalType::kTime: {
            json[name] = value_.time.ToString();
            return;
        }
        case LogicalType::kDateTime: {
            json[name] = value_.datetime.ToString();
            return;
        }
        case LogicalType::kTimestamp: {
            json[name] = value_.timestamp.ToString();
            return;
        }
        case LogicalType::kInterval: {
            json[name] = value_.interval.ToString();
            return;
        }
        case LogicalType::kRowID: {
            json[name] = value_.row.ToString();
            return;
        }
        case LogicalType::kVarchar: {
            json[name] = value_info_->Get<StringValueInfo>().GetString();
            return;
        }
        case LogicalType::kEmbedding: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_.type_info().get());
            Span<char> data_span = this->GetEmbedding();
            if (data_span.size() != embedding_info->Size()) {
                UnrecoverableError("Embedding data size mismatch.");
            }
            const EmbeddingT embedding(data_span.data(), false);
            Embedding2Json(embedding, embedding_info->Type(), embedding_info->Dimension(), json[name]);
            return;
        }
        case LogicalType::kMultiVector:
        case LogicalType::kTensor: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_.type_info().get());
            Tensor2Json(this->GetEmbedding(), embedding_info->Type(), embedding_info->Dimension(), json[name]);
            return;
        }
        case LogicalType::kTensorArray: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_.type_info().get());
            const auto &embedding_value_infos = this->GetTensorArray();
            Vector<Span<char>> tensor_array;
            tensor_array.reserve(embedding_value_infos.size());
            for (const auto &embedding_value_info : embedding_value_infos) {
                tensor_array.emplace_back(embedding_value_info->GetData());
            }
            TensorArray2Json(tensor_array, embedding_info->Type(), embedding_info->Dimension(), json[name]);
            return;
        }
        case LogicalType::kEmptyArray: {
            json[name] = "[]";
            return;
        }
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<const SparseInfo *>(type_.type_info().get());
            const auto &sparse_value_info = static_cast<const SparseValueInfo &>(*value_info_);

            nlohmann::json sparse_json;
            Sparse2Json(sparse_value_info, sparse_info, sparse_json);
            json[name] = sparse_json;
            return;
        }
        case LogicalType::kHugeInt:
        case LogicalType::kDecimal:
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kInvalid: {
            String error_message = fmt::format("Value::AppendToJson() not implemented for type {}", type_.ToString());
            UnrecoverableError(error_message);
        }
    }
}

void Value::AppendToArrowArray(const SharedPtr<DataType> &data_type, SharedPtr<arrow::ArrayBuilder> &array_builder) {
    switch (data_type->type()) {
        case LogicalType::kBoolean: {
            auto builder = std::dynamic_pointer_cast<::arrow::BooleanBuilder>(array_builder);
            auto status = builder->Append(value_.boolean);
            break;
        }
        case LogicalType::kTinyInt: {
            auto builder = std::dynamic_pointer_cast<::arrow::Int8Builder>(array_builder);
            auto status = builder->Append(value_.tiny_int);
            break;
        }
        case LogicalType::kSmallInt: {
            auto builder = std::dynamic_pointer_cast<::arrow::Int16Builder>(array_builder);
            auto status = builder->Append(value_.small_int);
            break;
        }
        case LogicalType::kInteger: {
            auto builder = std::dynamic_pointer_cast<::arrow::Int32Builder>(array_builder);
            auto status = builder->Append(value_.integer);
            break;
        }
        case LogicalType::kBigInt: {
            auto builder = std::dynamic_pointer_cast<::arrow::Int64Builder>(array_builder);
            auto status = builder->Append(value_.big_int);
            break;
        }
        case LogicalType::kFloat16: {
            auto builder = std::dynamic_pointer_cast<::arrow::HalfFloatBuilder>(array_builder);
            auto status = builder->Append(value_.float16.raw);
            break;
        }
        case LogicalType::kBFloat16: {
            auto builder = std::dynamic_pointer_cast<::arrow::FloatBuilder>(array_builder);
            auto status = builder->Append(static_cast<f32>(value_.bfloat16));
            break;
        }
        case LogicalType::kFloat: {
            auto builder = std::dynamic_pointer_cast<::arrow::FloatBuilder>(array_builder);
            auto status = builder->Append(value_.float32);
            break;
        }
        case LogicalType::kDouble: {
            auto builder = std::dynamic_pointer_cast<::arrow::DoubleBuilder>(array_builder);
            auto status = builder->Append(value_.float64);
            break;
        }
        case LogicalType::kDate: {
            auto builder = std::dynamic_pointer_cast<::arrow::Date32Builder>(array_builder);
            auto status = builder->Append(value_.date.value);
            break;
        }
        case LogicalType::kTime: {
            auto builder = std::dynamic_pointer_cast<::arrow::Time32Builder>(array_builder);
            auto status = builder->Append(value_.time.value);
            break;
        }
        case LogicalType::kDateTime: {
            auto builder = std::dynamic_pointer_cast<::arrow::TimestampBuilder>(array_builder);
            auto status = builder->Append(value_.datetime.GetEpochTime());
            break;
        }
        case LogicalType::kTimestamp: {
            auto builder = std::dynamic_pointer_cast<::arrow::TimestampBuilder>(array_builder);
            auto status = builder->Append(value_.timestamp.GetEpochTime());
            break;
        }
        case LogicalType::kVarchar: {
            auto builder = std::dynamic_pointer_cast<::arrow::StringBuilder>(array_builder);
            auto status = builder->Append(value_info_->Get<StringValueInfo>().GetString());
            break;
        }
        case LogicalType::kEmbedding: {
            auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
            Span<char> data_span = this->GetEmbedding();
            if (data_span.size() != embedding_info->Size()) {
                String error_message = "Embedding data size mismatch.";
                UnrecoverableError(error_message);
            }
            const EmbeddingT embedding(const_cast<char *>(data_span.data()), false);

            auto *fixedlist_builder = dynamic_cast<arrow::FixedSizeListBuilder *>(array_builder.get());
            auto status = fixedlist_builder->Append();

            Embedding2Arrow(embedding, embedding_info->Type(), embedding_info->Dimension(), fixedlist_builder->value_builder());
            break;
        }
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<const SparseInfo *>(data_type->type_info().get());
            auto struct_builder = std::static_pointer_cast<arrow::StructBuilder>(array_builder);
            auto status = struct_builder->Append();

            auto *index_builder = struct_builder->child(0);
            auto *value_builder = struct_builder->child(1);
            auto [nnz, index, data] = this->GetSparse();

            EmbeddingT index_embedding(index.data(), false);
            EmbeddingT data_embedding(data.data(), false);
            auto *list_index_builder = dynamic_cast<arrow::ListBuilder *>(index_builder);
            status = list_index_builder->Append();
            Embedding2Arrow(index_embedding, sparse_info->IndexType(), nnz, list_index_builder->value_builder());
            if (value_builder) {
                auto *list_value_builder = dynamic_cast<arrow::ListBuilder *>(value_builder);
                status = list_value_builder->Append();
                Embedding2Arrow(data_embedding, sparse_info->DataType(), nnz, list_value_builder->value_builder());
            }
            break;
        }
        case LogicalType::kMultiVector:
        case LogicalType::kTensor: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type->type_info().get());

            Span<char> tensor = this->GetEmbedding();

            auto *list_builder = dynamic_cast<arrow::ListBuilder *>(array_builder.get());
            auto status = list_builder->Append();
            Tensor2Arrow(tensor, embedding_info, list_builder->value_builder());
            break;
        }
        case LogicalType::kTensorArray: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type->type_info().get());

            const Vector<SharedPtr<EmbeddingValueInfo>> &embedding_value_infos = this->GetTensorArray();
            Vector<Span<char>> tensor_array;
            for (const auto &embedding_value_info : embedding_value_infos) {
                tensor_array.push_back(embedding_value_info->GetData());
            }

            auto *list_builder = dynamic_cast<arrow::ListBuilder *>(array_builder.get());
            auto status = list_builder->Append();
            TensorArray2Arrow(tensor_array, embedding_info, list_builder->value_builder());
            break;
        }
        case LogicalType::kRowID:
        case LogicalType::kInterval:
        case LogicalType::kHugeInt:
        case LogicalType::kDecimal:
        case LogicalType::kArray:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kEmptyArray:
        case LogicalType::kInvalid: {
            String error_message = "Invalid data type";
            UnrecoverableError(error_message);
        }
    }
}

SharedPtr<EmbeddingValueInfo> EmbeddingValueInfo::MakeTensorValueInfo(const_ptr_t ptr, SizeT bytes) {
    if (bytes == 0) {
        UnrecoverableError("EmbeddingValueInfo::MakeTensorValueInfo(bytes=0) is invalid.");
    }
    return MakeShared<EmbeddingValueInfo>(ptr, bytes);
}

SharedPtr<EmbeddingValueInfo> EmbeddingValueInfo::MakeTensorValueInfo(const Vector<Pair<ptr_t, SizeT>> &ptr_bytes) {
    return MakeShared<EmbeddingValueInfo>(ptr_bytes);
}

SharedPtr<EmbeddingValueInfo> EmbeddingValueInfo::MakeMultiVectorValueInfo(const_ptr_t ptr, SizeT bytes) { return MakeTensorValueInfo(ptr, bytes); }

SharedPtr<EmbeddingValueInfo> EmbeddingValueInfo::MakeMultiVectorValueInfo(const Vector<Pair<ptr_t, SizeT>> &ptr_bytes) {
    return MakeTensorValueInfo(ptr_bytes);
}

} // namespace infinity
