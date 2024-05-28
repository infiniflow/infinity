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

export module sparse_cast;

import stl;
import bound_cast_func;
import data_type;
import logical_type;
import status;
import infinity_exception;
import sparse_info;
import column_vector_cast;
import internal_types;
import column_vector;
import third_party;
import vector_buffer;
import fix_heap;
import sparse_info;
import embedding_cast;

namespace infinity {

struct SparseTryCastToSparse;

export inline BoundCastFunc BindSparseCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kSparse || target.type() != LogicalType::kSparse) {
        RecoverableError(Status::NotSupportedTypeConversion(source.ToString(), target.ToString()));
    }
    return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorVarlenWithType<SparseT, SparseT, SparseTryCastToSparse>);
}

struct SparseTryCastToSparse {
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

template <typename TargetValueType, typename TargetIndiceType, typename SourceValueType, typename SourceIndiceType>
void SparseTryCastToSparseFunInner(const SparseInfo *source_info,
                                   const SparseT &source,
                                   FixHeapManager *source_fix_heap_mgr,
                                   const SparseInfo *target_info,
                                   SparseT &target,
                                   FixHeapManager *target_fix_heap_mgr) {
    const auto &[source_nnz, source_chunk_id, source_chunk_offset] = source;
    target.nnz_ = source_nnz;
    const_ptr_t source_ptr = source_fix_heap_mgr->GetRawPtrFromChunk(source_chunk_id, source_chunk_offset);
    SizeT sparse_bytes = source_info->SparseSize(source_nnz);
    if constexpr (std::is_same_v<TargetValueType, SourceValueType>) {
        if constexpr (std::is_same_v<TargetIndiceType, SourceIndiceType>) {
            std::tie(target.chunk_id_, target.chunk_offset_) = target_fix_heap_mgr->AppendToHeap(source_ptr, sparse_bytes);
        } else {
            auto target_indice_tmp_ptr = MakeUniqueForOverwrite<TargetIndiceType[]>(source_nnz);
            const SizeT target_indice_size = target_info->IndiceSize(source_nnz);
            if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceIndiceType *>(source_ptr),
                                               reinterpret_cast<TargetIndiceType *>(target_indice_tmp_ptr.get()),
                                               source_nnz)) {
                UnrecoverableError(fmt::format("Fail to case from sparse with idx {} to sparse with idx {}",
                                               DataType::TypeToString<SourceIndiceType>(),
                                               DataType::TypeToString<TargetIndiceType>()));
            }
            Vector<Pair<const_ptr_t, SizeT>> data_ptrs;
            data_ptrs.emplace_back(reinterpret_cast<const char *>(target_indice_tmp_ptr.get()), target_indice_size);

            const SizeT source_indice_size = source_info->IndiceSize(source_nnz);
            const_ptr_t source_data_ptr = source_ptr + source_indice_size;
            data_ptrs.emplace_back(source_data_ptr, source_info->DataSize(source_nnz));

            std::tie(target.chunk_id_, target.chunk_offset_) = target_fix_heap_mgr->AppendToHeap(data_ptrs);
        }
    } else {
        const SizeT source_indice_size = source_info->IndiceSize(source_nnz);
        Vector<Pair<const_ptr_t, SizeT>> data_ptrs;
        UniquePtr<TargetIndiceType[]> target_indice_tmp_ptr;
        if constexpr (std::is_same_v<TargetIndiceType, SourceIndiceType>) {
            data_ptrs.emplace_back(reinterpret_cast<const char *>(source_ptr), source_indice_size);
        } else {
            target_indice_tmp_ptr = MakeUniqueForOverwrite<TargetIndiceType[]>(source_nnz);
            const SizeT target_indice_size = target_info->IndiceSize(source_nnz);
            if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceIndiceType *>(source_ptr),
                                               reinterpret_cast<TargetIndiceType *>(target_indice_tmp_ptr.get()),
                                               source_nnz)) {
                UnrecoverableError(fmt::format("Fail to case from sparse with idx {} to sparse with idx {}",
                                               DataType::TypeToString<SourceIndiceType>(),
                                               DataType::TypeToString<TargetIndiceType>()));
            }
            data_ptrs.emplace_back(reinterpret_cast<const char *>(target_indice_tmp_ptr.get()), target_indice_size);
        }
        auto target_value_tmp_ptr = MakeUniqueForOverwrite<TargetValueType[]>(source_nnz);
        const SizeT target_data_size = target_info->DataSize(source_nnz);
        if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceValueType *>(source_ptr + source_indice_size),
                                           reinterpret_cast<TargetValueType *>(target_value_tmp_ptr.get()),
                                           source_nnz)) {
            UnrecoverableError(fmt::format("Fail to case from sparse with idx {} to sparse with idx {}",
                                           DataType::TypeToString<SourceValueType>(),
                                           DataType::TypeToString<TargetValueType>()));
        }
        data_ptrs.emplace_back(reinterpret_cast<const char *>(target_value_tmp_ptr.get()), target_data_size);
        std::tie(target.chunk_id_, target.chunk_offset_) = target_fix_heap_mgr->AppendToHeap(data_ptrs);
    }
}

template <typename TargetValueType, typename TargetIndiceType, typename SourceValueType, typename SourceIndiceType>
void SparseTryCastToSparseFunT4(const SparseInfo *source_info,
                                const SparseT &source,
                                ColumnVector *source_vector_ptr,
                                const SparseInfo *target_info,
                                SparseT &target,
                                ColumnVector *target_vector_ptr) {
    SparseTryCastToSparseFunInner<TargetValueType, TargetIndiceType, SourceValueType, SourceIndiceType>(
        source_info,
        source,
        source_vector_ptr->buffer_->fix_heap_mgr_.get(),
        target_info,
        target,
        target_vector_ptr->buffer_->fix_heap_mgr_.get());
}

template <typename TargetValueType, typename TargetIndiceType, typename SourceValueType>
void SparseTryCastToSparseFunT3(const SparseInfo *source_info,
                                const SparseT &source,
                                ColumnVector *source_vector_ptr,
                                const SparseInfo *target_info,
                                SparseT &target,
                                ColumnVector *target_vector_ptr) {
    switch (source_info->IndexType()) {
        case kElemInt8: {
            SparseTryCastToSparseFunT4<TargetValueType, TargetIndiceType, SourceValueType, TinyIntT>(source_info,
                                                                                                     source,
                                                                                                     source_vector_ptr,
                                                                                                     target_info,
                                                                                                     target,
                                                                                                     target_vector_ptr);
            break;
        }
        case kElemInt16: {
            SparseTryCastToSparseFunT4<TargetValueType, TargetIndiceType, SourceValueType, SmallIntT>(source_info,
                                                                                                      source,
                                                                                                      source_vector_ptr,
                                                                                                      target_info,
                                                                                                      target,
                                                                                                      target_vector_ptr);
            break;
        }
        case kElemInt32: {
            SparseTryCastToSparseFunT4<TargetValueType, TargetIndiceType, SourceValueType, IntegerT>(source_info,
                                                                                                     source,
                                                                                                     source_vector_ptr,
                                                                                                     target_info,
                                                                                                     target,
                                                                                                     target_vector_ptr);
            break;
        }
        case kElemInt64: {
            SparseTryCastToSparseFunT4<TargetValueType, TargetIndiceType, SourceValueType, BigIntT>(source_info,
                                                                                                    source,
                                                                                                    source_vector_ptr,
                                                                                                    target_info,
                                                                                                    target,
                                                                                                    target_vector_ptr);
            break;
        }
        default: {
            UnrecoverableError("Invalid source index type");
        }
    }
}

template <typename TargetValueType, typename TargetIndiceType>
void SparseTryCastToSparseFunT2(const SparseInfo *source_info,
                                const SparseT &source,
                                ColumnVector *source_vector_ptr,
                                const SparseInfo *target_info,
                                SparseT &target,
                                ColumnVector *target_vector_ptr) {
    switch (source_info->DataType()) {
        case kElemBit:
        case kElemInt8:
        case kElemInt16:
        case kElemInt32:
        case kElemInt64:
        case kElemInvalid: {
            UnrecoverableError("Unimplemented");
        }
        case kElemFloat: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, FloatT>(source_info,
                                                                                  source,
                                                                                  source_vector_ptr,
                                                                                  target_info,
                                                                                  target,
                                                                                  target_vector_ptr);
            break;
        }
        case kElemDouble: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, DoubleT>(source_info,
                                                                                   source,
                                                                                   source_vector_ptr,
                                                                                   target_info,
                                                                                   target,
                                                                                   target_vector_ptr);
            break;
        }
        default: {
            UnrecoverableError("Unreachable code");
        }
    }
}

template <typename TargetValueType>
void SparseTryCastToSparseFunT1(const SparseInfo *source_info,
                                const SparseT &source,
                                ColumnVector *source_vector_ptr,
                                const SparseInfo *target_info,
                                SparseT &target,
                                ColumnVector *target_vector_ptr) {
    switch (target_info->IndexType()) {
        case kElemInt8: {
            SparseTryCastToSparseFunT2<TargetValueType, TinyIntT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemInt16: {
            SparseTryCastToSparseFunT2<TargetValueType, SmallIntT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemInt32: {
            SparseTryCastToSparseFunT2<TargetValueType, IntegerT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemInt64: {
            SparseTryCastToSparseFunT2<TargetValueType, BigIntT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        default: {
            UnrecoverableError("Invalid target index type");
        }
    }
}

void SparseTryCastToSparseFun(const SparseInfo *source_info,
                              const SparseT &source,
                              ColumnVector *source_vector_ptr,
                              const SparseInfo *target_info,
                              SparseT &target,
                              ColumnVector *target_vector_ptr) {
    switch (target_info->DataType()) {
        case kElemBit:
        case kElemInt8:
        case kElemInt16:
        case kElemInt32:
        case kElemInt64:
        case kElemInvalid: {
            UnrecoverableError("Unimplemented");
        }
        case kElemFloat: {
            SparseTryCastToSparseFunT1<FloatT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemDouble: {
            SparseTryCastToSparseFunT1<DoubleT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        default: {
            UnrecoverableError("Unreachable code");
        }
    }
}

template <>
bool SparseTryCastToSparse::Run(const SparseT &source,
                                const DataType &source_type,
                                ColumnVector *source_vector_ptr,
                                SparseT &target,
                                const DataType &target_type,
                                ColumnVector *target_vector_ptr) {
    const auto *source_info = static_cast<const SparseInfo *>(source_type.type_info().get());
    const auto *target_info = static_cast<const SparseInfo *>(target_type.type_info().get());
    SizeT source_dim = source_info->Dimension();
    SizeT target_dim = target_info->Dimension();
    if (source_dim > target_dim) {
        RecoverableError(Status::DataTypeMismatch(source_type.ToString(), target_type.ToString()));
    }
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kSparseHeap) {
        UnrecoverableError(fmt::format("Sparse column vector should use kHeap VectorBuffer."));
    }
    SparseTryCastToSparseFun(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
    return true;
}

} // namespace infinity