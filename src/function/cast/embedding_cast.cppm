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

export module embedding_cast;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;

import column_vector_cast;
import float_cast;
import integer_cast;
import infinity_exception;
import third_party;
import logger;
import status;
import logical_type;
import internal_types;
import embedding_info;
import sparse_info;
import knn_expr;
import data_type;
import default_values;
import sparse_util;

namespace infinity {

export struct EmbeddingTryCastToFixlen;
struct EmbeddingTryCastToVarlen;

template <typename SourceElemType>
BoundCastFunc BindEmbeddingCast(const EmbeddingInfo *target);

export inline BoundCastFunc BindEmbeddingCast(const DataType &source, const DataType &target) {
    if (source.type() == LogicalType::kEmbedding && target.type() == LogicalType::kVarchar) {
        return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlenWithType<EmbeddingT, VarcharT, EmbeddingTryCastToVarlen>);
    }
    if (source.type() == LogicalType::kEmbedding && target.type() == LogicalType::kTensor) {
        return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlenWithType<EmbeddingT, TensorT, EmbeddingTryCastToVarlen>);
    }
    if (source.type() == LogicalType::kEmbedding && target.type() == LogicalType::kMultiVector) {
        return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlenWithType<EmbeddingT, MultiVectorT, EmbeddingTryCastToVarlen>);
    }
    if (source.type() == LogicalType::kEmbedding && target.type() == LogicalType::kSparse) {
        auto *sparse_info = static_cast<const SparseInfo *>(target.type_info().get());
        if (sparse_info->DataType() == EmbeddingDataType::kElemBit) {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlenWithType<EmbeddingT, SparseT, EmbeddingTryCastToVarlen>);
        }
    }

    if (source.type() != LogicalType::kEmbedding || target.type() != LogicalType::kEmbedding) {
        Status status = Status::NotSupportedTypeConversion(source.ToString(), target.ToString());
        RecoverableError(status);
    }
    auto source_info = static_cast<const EmbeddingInfo *>(source.type_info().get());
    auto target_info = static_cast<const EmbeddingInfo *>(target.type_info().get());
    if (source_info->Dimension() != target_info->Dimension()) {
        Status status = Status::DataTypeMismatch(source.ToString(), target.ToString());
        RecoverableError(status);
    }
    switch (source_info->Type()) {
        case EmbeddingDataType::kElemBit: {
            return BindEmbeddingCast<BooleanT>(target_info);
        }
        case EmbeddingDataType::kElemUInt8: {
            return BindEmbeddingCast<u8>(target_info);
        }
        case EmbeddingDataType::kElemInt8: {
            return BindEmbeddingCast<TinyIntT>(target_info);
        }
        case EmbeddingDataType::kElemInt16: {
            return BindEmbeddingCast<SmallIntT>(target_info);
        }
        case EmbeddingDataType::kElemInt32: {
            return BindEmbeddingCast<IntegerT>(target_info);
        }
        case EmbeddingDataType::kElemInt64: {
            return BindEmbeddingCast<BigIntT>(target_info);
        }
        case EmbeddingDataType::kElemFloat: {
            return BindEmbeddingCast<FloatT>(target_info);
        }
        case EmbeddingDataType::kElemDouble: {
            return BindEmbeddingCast<DoubleT>(target_info);
        }
        case EmbeddingDataType::kElemFloat16: {
            return BindEmbeddingCast<Float16T>(target_info);
        }
        case EmbeddingDataType::kElemBFloat16: {
            return BindEmbeddingCast<BFloat16T>(target_info);
        }
        case EmbeddingDataType::kElemInvalid: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

template <typename SourceElemType>
inline BoundCastFunc BindEmbeddingCast(const EmbeddingInfo *target) {
    switch (target->Type()) {
        case EmbeddingDataType::kElemBit: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, BooleanT, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemUInt8: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, u8, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemInt8: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, TinyIntT, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemInt16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, SmallIntT, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemInt32: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, IntegerT, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemInt64: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, BigIntT, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemFloat: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, FloatT, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemDouble: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, DoubleT, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemFloat16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, Float16T, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemBFloat16: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorEmbedding<SourceElemType, BFloat16T, EmbeddingTryCastToFixlen>);
        }
        case EmbeddingDataType::kElemInvalid: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct EmbeddingTryCastToFixlen {
    template <typename SourceElemType, typename TargetElemType>
    static inline bool Run(const SourceElemType *source, TargetElemType *target, SizeT len) {
        if constexpr (std::is_same_v<TargetElemType, bool>) {
            if constexpr (!(std::is_same_v<SourceElemType, u8> || std::is_same_v<SourceElemType, TinyIntT> ||
                            std::is_same_v<SourceElemType, SmallIntT> || std::is_same_v<SourceElemType, IntegerT> ||
                            std::is_same_v<SourceElemType, BigIntT> || std::is_same_v<SourceElemType, FloatT> ||
                            std::is_same_v<SourceElemType, DoubleT> || std::is_same_v<SourceElemType, Float16T> ||
                            std::is_same_v<SourceElemType, BFloat16T>)) {
                String error_message = fmt::format("Not support to cast from {} to {}",
                                                   DataType::TypeToString<SourceElemType>(),
                                                   DataType::TypeToString<TargetElemType>());
                UnrecoverableError(error_message);
            }
            auto *dst = reinterpret_cast<u8 *>(target);
            std::fill_n(dst, (len + 7) / 8, 0);
            for (SizeT i = 0; i < len; ++i) {
                if (source[i]) {
                    dst[i / 8] |= (1u << (i % 8));
                }
            }
            return true;
        } else if constexpr (std::is_same_v<SourceElemType, bool>) {
            if constexpr (!(std::is_same_v<TargetElemType, u8> || std::is_same_v<TargetElemType, TinyIntT> ||
                            std::is_same_v<TargetElemType, SmallIntT> || std::is_same_v<TargetElemType, IntegerT> ||
                            std::is_same_v<TargetElemType, BigIntT> || std::is_same_v<TargetElemType, FloatT> ||
                            std::is_same_v<TargetElemType, DoubleT> || std::is_same_v<TargetElemType, Float16T> ||
                            std::is_same_v<TargetElemType, BFloat16T>)) {
                String error_message = fmt::format("Not support to cast from {} to {}",
                                                   DataType::TypeToString<SourceElemType>(),
                                                   DataType::TypeToString<TargetElemType>());
                UnrecoverableError(error_message);
            }
            const auto *src = reinterpret_cast<const u8 *>(source);
            for (SizeT i = 0; i < len; ++i) {
                if constexpr (std::is_same_v<TargetElemType, u8> || std::is_same_v<TargetElemType, TinyIntT> ||
                              std::is_same_v<TargetElemType, SmallIntT> || std::is_same_v<TargetElemType, IntegerT> ||
                              std::is_same_v<TargetElemType, BigIntT>) {
                    target[i] = (src[i / 8] & (1u << (i % 8))) ? 1 : 0;
                } else if constexpr (std::is_same_v<TargetElemType, FloatT> || std::is_same_v<TargetElemType, DoubleT> ||
                                     std::is_same_v<TargetElemType, Float16T> || std::is_same_v<TargetElemType, BFloat16T>) {
                    target[i] = (src[i / 8] & (1u << (i % 8))) ? 1.0f : 0.0f;
                } else {
                    static_assert(false, "Some case not handled!");
                }
            }
            return true;
        } else if constexpr (std::is_same<SourceElemType, u8>() || std::is_same<SourceElemType, TinyIntT>() ||
                             std::is_same<SourceElemType, SmallIntT>() || std::is_same<SourceElemType, IntegerT>() ||
                             std::is_same<SourceElemType, BigIntT>()) {
            for (SizeT i = 0; i < len; ++i) {
                if (!IntegerTryCastToFixlen::Run(source[i], target[i])) {
                    return false;
                }
            }
            return true;
        } else if constexpr (std::is_same<SourceElemType, FloatT>() || std::is_same<SourceElemType, DoubleT>() ||
                             std::is_same_v<SourceElemType, Float16T> || std::is_same_v<SourceElemType, BFloat16T>) {
            for (SizeT i = 0; i < len; ++i) {
                if (!FloatTryCastToFixlen::Run(source[i], target[i])) {
                    return false;
                }
            }
            return true;
        } else {
            String error_message =
                fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceElemType>(), DataType::TypeToString<TargetElemType>());
            UnrecoverableError(error_message);
            return false;
        }
    }
};

struct EmbeddingTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &, const DataType &, TargetType &, const DataType &, ColumnVector *) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          VarcharT &target,
                                          const DataType &target_type,
                                          ColumnVector *vector_ptr) {
    if (source_type.type() != LogicalType::kEmbedding) {
        String error_message = fmt::format("Type here is expected as Embedding, but actually it is: {}", source_type.ToString());
        UnrecoverableError(error_message);
    }

    EmbeddingInfo *embedding_info = (EmbeddingInfo *)(source_type.type_info().get());

    LOG_TRACE(fmt::format("EmbeddingInfo Dimension: {}", embedding_info->Dimension()));

    String res = EmbeddingT::Embedding2String(source, embedding_info->Type(), embedding_info->Dimension());
    vector_ptr->AppendVarcharInner({res.data(), res.size()}, target);

    return true;
}

export void ColumnVectorSetResult(TensorT &target, auto &&...args) { ColumnVector::SetTensor(target, std::forward<decltype(args)>(args)...); }

export void ColumnVectorSetResult(MultiVectorT &target, auto &&...args) { ColumnVector::SetMultiVector(target, std::forward<decltype(args)>(args)...); }

template <typename TargetValueType, typename SourceValueType>
void EmbeddingTryCastToTensorMultiVectorImpl(const EmbeddingT &source,
                                             const SizeT source_embedding_dim,
                                             auto &target,
                                             ColumnVector *target_vector_ptr) {
    using TargetT = std::decay_t<decltype(target)>;
    static_assert(std::is_same_v<TargetT, TensorT> || std::is_same_v<TargetT, MultiVectorT>);
    const auto *target_info = static_cast<const EmbeddingInfo *>(target_vector_ptr->data_type()->type_info().get());
    if constexpr (std::is_same_v<TargetValueType, SourceValueType>) {
        const auto source_size =
            std::is_same_v<SourceValueType, BooleanT> ? (source_embedding_dim + 7) / 8 : source_embedding_dim * sizeof(SourceValueType);
        Span<const char> tensor_data = {source.ptr, source_size};
        ColumnVectorSetResult(target, target_vector_ptr->buffer_.get(), tensor_data, target_info);
    } else if constexpr (std::is_same_v<TargetValueType, BooleanT>) {
        static_assert(sizeof(bool) == 1);
        const auto target_size = (source_embedding_dim + 7) / 8;
        auto target_tmp_ptr = MakeUnique<char[]>(target_size);
        auto src_ptr = reinterpret_cast<const SourceValueType *>(source.ptr);
        for (SizeT i = 0; i < source_embedding_dim; ++i) {
            if (src_ptr[i]) {
                target_tmp_ptr[i / 8] |= (1u << (i % 8));
            }
        }
        Span<const char> tensor_data = {target_tmp_ptr.get(), target_size};
        ColumnVectorSetResult(target, target_vector_ptr->buffer_.get(), tensor_data, target_info);
    } else {
        const auto target_size = source_embedding_dim * sizeof(TargetValueType);
        auto target_tmp_ptr = MakeUniqueForOverwrite<TargetValueType[]>(source_embedding_dim);
        if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceValueType *>(source.ptr),
                                           reinterpret_cast<TargetValueType *>(target_tmp_ptr.get()),
                                           source_embedding_dim)) {
            UnrecoverableError(fmt::format("Failed to cast from embedding with type {} to tensor with type {}",
                                           DataType::TypeToString<SourceValueType>(),
                                           DataType::TypeToString<TargetValueType>()));
        }
        Span<const char> tensor_data = {reinterpret_cast<const char *>(target_tmp_ptr.get()), target_size};
        ColumnVectorSetResult(target, target_vector_ptr->buffer_.get(), tensor_data, target_info);
    }
}

template <typename... T>
void EmbeddingTryCastToTensorMultiVector(const EmbeddingT &source, const SizeT source_embedding_dim, auto &target, ColumnVector *target_vector_ptr) {
    return EmbeddingTryCastToTensorMultiVectorImpl<T...>(source, source_embedding_dim, target, target_vector_ptr);
}

template <typename... T>
void EmbeddingTryCastToTensorMultiVector(const EmbeddingT &source,
                                         const SizeT source_embedding_dim,
                                         auto &target,
                                         ColumnVector *target_vector_ptr,
                                         const EmbeddingDataType emb_data_type,
                                         const auto... args) {
    switch (emb_data_type) {
        case EmbeddingDataType::kElemBit: {
            EmbeddingTryCastToTensorMultiVector<BooleanT, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            EmbeddingTryCastToTensorMultiVector<u8, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            EmbeddingTryCastToTensorMultiVector<TinyIntT, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            EmbeddingTryCastToTensorMultiVector<SmallIntT, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            EmbeddingTryCastToTensorMultiVector<IntegerT, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            EmbeddingTryCastToTensorMultiVector<BigIntT, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            EmbeddingTryCastToTensorMultiVector<FloatT, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            EmbeddingTryCastToTensorMultiVector<DoubleT, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            EmbeddingTryCastToTensorMultiVector<Float16T, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            EmbeddingTryCastToTensorMultiVector<BFloat16T, T...>(source, source_embedding_dim, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Unreachable code");
        }
    }
}

inline bool EmbeddingTryCastToVarlenTensorMultiVector(const EmbeddingT &source,
                                                      const DataType &source_type,
                                                      auto &target,
                                                      const DataType &target_type,
                                                      ColumnVector *target_vector_ptr) {
    if (source_type.type() != LogicalType::kEmbedding) {
        UnrecoverableError(fmt::format("Type here is expected as Embedding, but actually it is: {}", source_type.ToString()));
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(source_type.type_info().get());
    const auto target_embedding_info = static_cast<const EmbeddingInfo *>(target_type.type_info().get());
    LOG_TRACE(fmt::format("EmbeddingInfo Dimension: {}", embedding_info->Dimension()));
    const auto source_embedding_dim = embedding_info->Dimension();
    const auto target_embedding_dim = target_embedding_info->Dimension();
    if (source_embedding_dim % target_embedding_dim != 0 || source_embedding_dim == 0) {
        RecoverableError(Status::DataTypeMismatch(source_type.ToString(), target_type.ToString()));
    }
    const auto target_tensor_num = source_embedding_dim / target_embedding_dim;
    target.embedding_num_ = target_tensor_num;
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kVarBuffer) {
        UnrecoverableError(fmt::format("Tensor / MultiVector column should use kVarBuffer VectorBuffer."));
    }
    EmbeddingTryCastToTensorMultiVector<>(source,
                                          source_embedding_dim,
                                          target,
                                          target_vector_ptr,
                                          embedding_info->Type(),
                                          target_embedding_info->Type());
    return true;
}

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          TensorT &target,
                                          const DataType &target_type,
                                          ColumnVector *target_vector_ptr) {
    return EmbeddingTryCastToVarlenTensorMultiVector(source, source_type, target, target_type, target_vector_ptr);
}

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          MultiVectorT &target,
                                          const DataType &target_type,
                                          ColumnVector *target_vector_ptr) {
    return EmbeddingTryCastToVarlenTensorMultiVector(source, source_type, target, target_type, target_vector_ptr);
}

template <typename IdxT, typename SourceType>
void EmbeddingTryCastToSparseImpl(const EmbeddingT &source,
                                  const EmbeddingInfo *source_info,
                                  SparseT &target,
                                  const SparseInfo *target_info,
                                  ColumnVector *target_vector_ptr) {
    SizeT source_dim = source_info->Dimension();
    auto target_max_dim = static_cast<SourceType>(target_info->Dimension());
    {
        HashSet<IdxT> idx_set;
        const auto *source_ptr = reinterpret_cast<const SourceType *>(source.ptr);
        for (SizeT i = 0; i < source_dim; ++i) {
            if (source_ptr[i] >= target_max_dim || source_ptr[i] < 0) {
                RecoverableError(
                    Status::DataTypeMismatch(fmt::format("{} with data {}", source_info->ToString(), source_ptr[i]), target_info->ToString()));
            }
            auto [_, inserted] = idx_set.insert(static_cast<IdxT>(source_ptr[i]));
            if (!inserted) {
                RecoverableError(Status::InvalidDataType());
            }
        }
    }

    target.nnz_ = source_dim;
    if constexpr (std::is_same_v<IdxT, SourceType>) {
        target_vector_ptr->AppendSparseInner(source_dim, static_cast<const bool *>(nullptr), reinterpret_cast<const IdxT *>(source.ptr), target);
    } else {
        auto target_tmp_ptr = MakeUniqueForOverwrite<IdxT[]>(source_dim);
        if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceType *>(source.ptr), target_tmp_ptr.get(), source_dim)) {
            String error_message = fmt::format("Failed to cast from embedding with type {} to sparse with type {}",
                                               DataType::TypeToString<SourceType>(),
                                               DataType::TypeToString<IdxT>());
            UnrecoverableError(error_message);
        }
        target_vector_ptr->AppendSparseInner(source_dim, static_cast<const bool *>(nullptr), target_tmp_ptr.get(), target);
    }
}

template <typename IdxT>
void EmbeddingTryCastToSparse(const EmbeddingT &source,
                              const EmbeddingInfo *source_info,
                              SparseT &target,
                              const SparseInfo *target_info,
                              ColumnVector *target_vector_ptr) {
    switch (source_info->Type()) {
        case kElemInt8: {
            EmbeddingTryCastToSparseImpl<IdxT, TinyIntT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        case kElemInt16: {
            EmbeddingTryCastToSparseImpl<IdxT, SmallIntT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        case kElemInt32: {
            EmbeddingTryCastToSparseImpl<IdxT, IntegerT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        case kElemInt64: {
            EmbeddingTryCastToSparseImpl<IdxT, BigIntT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        default: {
            UnrecoverableError(
                fmt::format("Cannot cast from embedding with type {} to sparse", EmbeddingInfo::EmbeddingDataTypeToString(source_info->Type())));
        }
    }
}

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          SparseT &target,
                                          const DataType &target_type,
                                          ColumnVector *target_vector_ptr) {
    if (source_type.type() != LogicalType::kEmbedding) {
        String error_message = fmt::format("Type here is expected as Embedding, but actually it is: {}", source_type.ToString());
        UnrecoverableError(error_message);
    }
    const auto *source_info = static_cast<EmbeddingInfo *>(source_type.type_info().get());
    const auto *target_info = static_cast<SparseInfo *>(target_type.type_info().get());

    if (target_info->DataType() != EmbeddingDataType::kElemBit) {
        String error_message = fmt::format("No support data type: {}", EmbeddingType::EmbeddingDataType2String(target_info->IndexType()));
        UnrecoverableError(error_message);
    }
    switch (target_info->IndexType()) {
        case EmbeddingDataType::kElemInt8: {
            EmbeddingTryCastToSparse<TinyIntT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            EmbeddingTryCastToSparse<SmallIntT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            EmbeddingTryCastToSparse<IntegerT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            EmbeddingTryCastToSparse<BigIntT>(source, source_info, target, target_info, target_vector_ptr);
            break;
        }
        default: {
            String error_message = fmt::format("No support data type: {}", EmbeddingType::EmbeddingDataType2String(target_info->IndexType()));
            UnrecoverableError(error_message);
        }
    }
    return true;
}

} // namespace infinity
