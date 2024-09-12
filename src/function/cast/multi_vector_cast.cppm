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

export module multi_vector_cast;

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
import knn_expr;
import data_type;
import default_values;
import embedding_cast;

namespace infinity {

struct MultiVectorTryCastToMultiVector;

export inline BoundCastFunc BindMultiVectorCast(const DataType &source, const DataType &target) {
    // now we only support cast between two tensors of the same basic dimension
    if (source.type() != LogicalType::kMultiVector || target.type() != LogicalType::kMultiVector) {
        RecoverableError(Status::NotSupportedTypeConversion(source.ToString(), target.ToString()));
    }
    const auto source_info = static_cast<const EmbeddingInfo *>(source.type_info().get());
    const auto target_info = static_cast<const EmbeddingInfo *>(target.type_info().get());
    if (source_info->Dimension() != target_info->Dimension()) {
        RecoverableError(Status::DataTypeMismatch(source.ToString(), target.ToString()));
    }
    return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorVarlenWithType<MultiVectorT, MultiVectorT, MultiVectorTryCastToMultiVector>);
}

template <typename TargetValueType, typename SourceValueType>
void MultiVectorTryCastToMultiVectorImpl(const MultiVectorT &source,
                                         const ColumnVector *source_vector_ptr,
                                         MultiVectorT &target,
                                         ColumnVector *target_vector_ptr) {
    const auto *source_embedding_info = static_cast<const EmbeddingInfo *>(source_vector_ptr->data_type()->type_info().get());
    const auto *target_embedding_info = static_cast<const EmbeddingInfo *>(target_vector_ptr->data_type()->type_info().get());
    if (source_embedding_info->Dimension() != target_embedding_info->Dimension()) {
        RecoverableError(Status::DataTypeMismatch(source_vector_ptr->data_type()->ToString(), target_vector_ptr->data_type()->ToString()));
    }

    auto [raw_data, embedding_num] = ColumnVector::GetMultiVector(source, source_vector_ptr->buffer_.get(), source_embedding_info);
    SizeT source_total_dim = source_embedding_info->Dimension() * embedding_num;
    if constexpr (std::is_same_v<TargetValueType, SourceValueType>) {
        ColumnVector::SetMultiVector(target, target_vector_ptr->buffer_.get(), raw_data, target_embedding_info);
    } else if constexpr (std::is_same_v<TargetValueType, BooleanT>) {
        static_assert(sizeof(bool) == 1);
        const auto target_size = (source_total_dim + 7) / 8;
        auto target_tmp_ptr = MakeUnique<char[]>(target_size);
        auto src_ptr = reinterpret_cast<const SourceValueType *>(raw_data.data());
        for (SizeT i = 0; i < source_total_dim; ++i) {
            if (src_ptr[i]) {
                target_tmp_ptr[i / 8] |= (1u << (i % 8));
            }
        }
        ColumnVector::SetMultiVector(target, target_vector_ptr->buffer_.get(), {target_tmp_ptr.get(), target_size}, target_embedding_info);
    } else {
        const auto target_size = source_total_dim * sizeof(TargetValueType);
        auto target_tmp_ptr = MakeUniqueForOverwrite<TargetValueType[]>(source_total_dim);
        if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceValueType *>(raw_data.data()),
                                           reinterpret_cast<TargetValueType *>(target_tmp_ptr.get()),
                                           source_total_dim)) {
            UnrecoverableError(fmt::format("Failed to cast from tensor with type {} to tensor with type {}",
                                           DataType::TypeToString<SourceValueType>(),
                                           DataType::TypeToString<TargetValueType>()));
        }
        ColumnVector::SetMultiVector(target,
                                     target_vector_ptr->buffer_.get(),
                                     {reinterpret_cast<const char *>(target_tmp_ptr.get()), target_size},
                                     target_embedding_info);
    }
}

template <typename... T>
void MultiVectorTryCastToMultiVectorFun(const MultiVectorT &source,
                                        const ColumnVector *source_vector_ptr,
                                        MultiVectorT &target,
                                        ColumnVector *target_vector_ptr) {
    return MultiVectorTryCastToMultiVectorImpl<T...>(source, source_vector_ptr, target, target_vector_ptr);
}

template <typename... T>
void MultiVectorTryCastToMultiVectorFun(const MultiVectorT &source,
                                        const ColumnVector *source_vector_ptr,
                                        MultiVectorT &target,
                                        ColumnVector *target_vector_ptr,
                                        const EmbeddingDataType emb_data_type,
                                        const auto... args) {
    switch (emb_data_type) {
        case EmbeddingDataType::kElemBit: {
            MultiVectorTryCastToMultiVectorFun<BooleanT, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            MultiVectorTryCastToMultiVectorFun<u8, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            MultiVectorTryCastToMultiVectorFun<TinyIntT, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            MultiVectorTryCastToMultiVectorFun<SmallIntT, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            MultiVectorTryCastToMultiVectorFun<IntegerT, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            MultiVectorTryCastToMultiVectorFun<BigIntT, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            MultiVectorTryCastToMultiVectorFun<FloatT, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            MultiVectorTryCastToMultiVectorFun<DoubleT, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            MultiVectorTryCastToMultiVectorFun<Float16T, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            MultiVectorTryCastToMultiVectorFun<BFloat16T, T...>(source, source_vector_ptr, target, target_vector_ptr, args...);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Unreachable code");
        }
    }
}

struct MultiVectorTryCastToMultiVector {
    template <typename SourceT, typename TargetT>
    static bool Run(const SourceT &source,
                    const DataType &source_type,
                    const ColumnVector *source_vector_ptr,
                    TargetT &target,
                    const DataType &target_type,
                    ColumnVector *target_vector_ptr) {
        UnrecoverableError("Unreachable code");
        return false;
    }
};

template <>
bool MultiVectorTryCastToMultiVector::Run<MultiVectorT, MultiVectorT>(const MultiVectorT &source,
                                                                      const DataType &source_type,
                                                                      const ColumnVector *source_vector_ptr,
                                                                      MultiVectorT &target,
                                                                      const DataType &target_type,
                                                                      ColumnVector *target_vector_ptr) {
    const auto source_embedding_info = static_cast<const EmbeddingInfo *>(source_type.type_info().get());
    const auto target_embedding_info = static_cast<const EmbeddingInfo *>(target_type.type_info().get());
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kVarBuffer) {
        UnrecoverableError(fmt::format("MultiVector column vector should use kVarBuffer VectorBuffer."));
    }
    MultiVectorTryCastToMultiVectorFun<>(source,
                                         source_vector_ptr,
                                         target,
                                         target_vector_ptr,
                                         source_embedding_info->Type(),
                                         target_embedding_info->Type());
    return true;
}

} // namespace infinity
