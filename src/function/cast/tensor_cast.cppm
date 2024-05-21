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

export module tensor_cast;

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
import fix_heap;

namespace infinity {

struct TensorTryCastToTensor;

export inline BoundCastFunc BindTensorCast(const DataType &source, const DataType &target) {
    // now we only support cast between two tensors of the same basic dimension
    if (source.type() != LogicalType::kTensor || target.type() != LogicalType::kTensor) {
        RecoverableError(Status::NotSupportedTypeConversion(source.ToString(), target.ToString()));
    }
    auto source_info = static_cast<const EmbeddingInfo *>(source.type_info().get());
    auto target_info = static_cast<const EmbeddingInfo *>(target.type_info().get());
    if (source_info->Dimension() != target_info->Dimension()) {
        RecoverableError(Status::DataTypeMismatch(source.ToString(), target.ToString()));
    }
    return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorVarlenWithType<TensorT, TensorT, TensorTryCastToTensor>);
}

export template <typename TargetValueType, typename SourceValueType>
void TensorTryCastToTensorImplInner(const u32 basic_embedding_dim,
                                    const TensorT &source,
                                    FixHeapManager *source_fix_heap_mgr,
                                    TensorT &target,
                                    FixHeapManager *target_fix_heap_mgr) {
    const auto [source_embedding_num, source_chunk_id, source_chunk_offset] = source;
    target.embedding_num_ = source_embedding_num;
    const auto source_total_dim = basic_embedding_dim * source_embedding_num;
    const auto source_ptr = source_fix_heap_mgr->GetRawPtrFromChunk(source_chunk_id, source_chunk_offset);
    if constexpr (std::is_same_v<TargetValueType, SourceValueType>) {
        const auto source_size = std::is_same_v<SourceValueType, BooleanT> ? (source_total_dim + 7) / 8 : source_total_dim * sizeof(SourceValueType);
        std::tie(target.chunk_id_, target.chunk_offset_) = target_fix_heap_mgr->AppendToHeap(source_ptr, source_size);
    } else if constexpr (std::is_same_v<TargetValueType, BooleanT>) {
        static_assert(sizeof(bool) == 1);
        const auto target_size = (source_total_dim + 7) / 8;
        auto target_tmp_ptr = MakeUnique<u8[]>(target_size);
        auto src_ptr = reinterpret_cast<const SourceValueType *>(source_ptr);
        for (SizeT i = 0; i < source_total_dim; ++i) {
            if (src_ptr[i]) {
                target_tmp_ptr[i / 8] |= (1u << (i % 8));
            }
        }
        std::tie(target.chunk_id_, target.chunk_offset_) =
            target_fix_heap_mgr->AppendToHeap(reinterpret_cast<const char *>(target_tmp_ptr.get()), target_size);
    } else {
        const auto target_size = source_total_dim * sizeof(TargetValueType);
        auto target_tmp_ptr = MakeUniqueForOverwrite<TargetValueType[]>(source_total_dim);
        if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceValueType *>(source_ptr),
                                           reinterpret_cast<TargetValueType *>(target_tmp_ptr.get()),
                                           source_total_dim)) {
            UnrecoverableError(fmt::format("Failed to cast from tensor with type {} to tensor with type {}",
                                           DataType::TypeToString<SourceValueType>(),
                                           DataType::TypeToString<TargetValueType>()));
        }
        std::tie(target.chunk_id_, target.chunk_offset_) =
            target_fix_heap_mgr->AppendToHeap(reinterpret_cast<const char *>(target_tmp_ptr.get()), target_size);
    }
}

template <typename TargetValueType, typename SourceValueType>
void TensorTryCastToTensorImpl(const u32 basic_embedding_dim,
                               const TensorT &source,
                               ColumnVector *source_vector_ptr,
                               TensorT &target,
                               ColumnVector *target_vector_ptr) {
    TensorTryCastToTensorImplInner<TargetValueType, SourceValueType>(basic_embedding_dim,
                                                                     source,
                                                                     source_vector_ptr->buffer_->fix_heap_mgr_.get(),
                                                                     target,
                                                                     target_vector_ptr->buffer_->fix_heap_mgr_.get());
}

template <typename TargetValueType>
void TensorTryCastToTensorImpl(const u32 basic_embedding_dim,
                               const TensorT &source,
                               const EmbeddingDataType src_type,
                               ColumnVector *source_vector_ptr,
                               TensorT &target,
                               ColumnVector *target_vector_ptr) {
    switch (src_type) {
        case EmbeddingDataType::kElemBit: {
            TensorTryCastToTensorImpl<TargetValueType, BooleanT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            TensorTryCastToTensorImpl<TargetValueType, TinyIntT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            TensorTryCastToTensorImpl<TargetValueType, SmallIntT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            TensorTryCastToTensorImpl<TargetValueType, IntegerT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            TensorTryCastToTensorImpl<TargetValueType, BigIntT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            TensorTryCastToTensorImpl<TargetValueType, FloatT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            TensorTryCastToTensorImpl<TargetValueType, DoubleT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        default: {
            UnrecoverableError("Unreachable code");
        }
    }
}

void TensorTryCastToTensorFun(const u32 basic_embedding_dim,
                              const TensorT &source,
                              const EmbeddingDataType src_type,
                              ColumnVector *source_vector_ptr,
                              TensorT &target,
                              const EmbeddingDataType dst_type,
                              ColumnVector *target_vector_ptr) {
    switch (dst_type) {
        case EmbeddingDataType::kElemBit: {
            TensorTryCastToTensorImpl<BooleanT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            TensorTryCastToTensorImpl<TinyIntT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            TensorTryCastToTensorImpl<SmallIntT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            TensorTryCastToTensorImpl<IntegerT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            TensorTryCastToTensorImpl<BigIntT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            TensorTryCastToTensorImpl<FloatT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            TensorTryCastToTensorImpl<DoubleT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Can't cast from embedding to tensor with type {}", EmbeddingInfo::EmbeddingDataTypeToString(dst_type)));
        }
    }
}

struct TensorTryCastToTensor {
    template <typename SourceT, typename TargetT>
    static bool Run(const SourceT &source,
                    const DataType &source_type,
                    ColumnVector *source_vector_ptr,
                    TargetT &target,
                    const DataType &target_type,
                    ColumnVector *target_vector_ptr) {
        UnrecoverableError("Unexpected case");
        return false;
    }
};

template <>
bool TensorTryCastToTensor::Run<TensorT, TensorT>(const TensorT &source,
                                                  const DataType &source_type,
                                                  ColumnVector *source_vector_ptr,
                                                  TensorT &target,
                                                  const DataType &target_type,
                                                  ColumnVector *target_vector_ptr) {
    const EmbeddingInfo *source_embedding_info = (EmbeddingInfo *)(source_type.type_info().get());
    const EmbeddingInfo *target_embedding_info = (EmbeddingInfo *)(target_type.type_info().get());
    const auto source_embedding_dim = source_embedding_info->Dimension();
    const auto target_embedding_dim = target_embedding_info->Dimension();
    if (source_embedding_dim != target_embedding_dim) {
        RecoverableError(Status::DataTypeMismatch(source_type.ToString(), target_type.ToString()));
    }
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kTensorHeap) {
        UnrecoverableError(fmt::format("Tensor column vector should use kTensorHeap VectorBuffer."));
    }
    TensorTryCastToTensorFun(source_embedding_dim,
                             source,
                             source_embedding_info->Type(),
                             source_vector_ptr,
                             target,
                             target_embedding_info->Type(),
                             target_vector_ptr);
    return true;
}

} // namespace infinity
