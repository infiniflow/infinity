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
import logger;
import sparse_util;

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
        String error_message = "Unexpected case";
        UnrecoverableError(error_message);
        return false;
    }
};

template <typename SourceValueType, typename SourceIndiceType>
SparseVec<SourceValueType, SourceIndiceType> SortSourceSparse(SparseVecRef<SourceValueType, SourceIndiceType> old_sparse_ref) {
    Vector<SizeT> idx(old_sparse_ref.nnz_);
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&old_sparse_ref](SizeT i, SizeT j) { return old_sparse_ref.indices_[i] < old_sparse_ref.indices_[j]; });
    auto new_data = MakeUniqueForOverwrite<SourceValueType[]>(old_sparse_ref.nnz_);
    auto new_indices = MakeUniqueForOverwrite<SourceIndiceType[]>(old_sparse_ref.nnz_);
    for (i32 i = 0; i < old_sparse_ref.nnz_; ++i) {
        new_data[i] = old_sparse_ref.data_[idx[i]];
        new_indices[i] = old_sparse_ref.indices_[idx[i]];
    }
    return {old_sparse_ref.nnz_, std::move(new_indices), std::move(new_data)};
}

template <typename TargetValueType, typename TargetIndiceType, typename SourceValueType, typename SourceIndiceType>
void SparseTryCastToSparseFunInner(const SparseInfo *source_info,
                                   const SparseT &source,
                                   const VectorBuffer *src_vec_buffer,
                                   const SparseInfo *target_info,
                                   SparseT &target,
                                   VectorBuffer *tgt_vec_buffer) {
    const auto &[source_nnz, source_file_offset] = source;
    target.nnz_ = source_nnz;
    if (source_nnz == 0) {
        target.file_offset_ = -1;
        return;
    }
    SparseVecRef<SourceValueType, SourceIndiceType> source_sparse_ref =
        src_vec_buffer->template GetSparse<SourceValueType, SourceIndiceType>(source_file_offset, source_nnz);
    SparseVec<SourceValueType, SourceIndiceType> source_tmp;
    if (target_info->StoreType() == SparseStoreType::kSort && source_info->StoreType() != SparseStoreType::kSort) {
        source_tmp = SortSourceSparse<SourceValueType, SourceIndiceType>(source_sparse_ref);
        source_sparse_ref = source_tmp.ToRef();
    }

    if constexpr (std::is_same_v<TargetValueType, SourceValueType>) {
        if constexpr (std::is_same_v<TargetIndiceType, SourceIndiceType>) {
            target.file_offset_ = tgt_vec_buffer->AppendSparse(source_sparse_ref);
        } else {
            auto target_indice_tmp_ptr = MakeUniqueForOverwrite<TargetIndiceType[]>(source_nnz);
            if (!EmbeddingTryCastToFixlen::Run(source_sparse_ref.indices_, target_indice_tmp_ptr.get(), source_nnz)) {
                String error_message = fmt::format("Fail to case from sparse with idx {} to sparse with idx {}",
                                                   DataType::TypeToString<SourceIndiceType>(),
                                                   DataType::TypeToString<TargetIndiceType>());
                UnrecoverableError(error_message);
            }
            SparseVecRef<TargetValueType, TargetIndiceType> target_sparse_ref{static_cast<i32>(source_nnz),
                                                                              target_indice_tmp_ptr.get(),
                                                                              source_sparse_ref.data_};
            target.file_offset_ = tgt_vec_buffer->AppendSparse(target_sparse_ref);
        }
    } else {
        UniquePtr<TargetValueType[]> target_value_tmp_ptr = nullptr;
        if constexpr (!std::is_same_v<TargetValueType, BooleanT>) {
            target_value_tmp_ptr = MakeUniqueForOverwrite<TargetValueType[]>(source_nnz);
            if (!EmbeddingTryCastToFixlen::Run(source_sparse_ref.data_, target_value_tmp_ptr.get(), source_nnz)) {
                String error_message = fmt::format("Fail to case from sparse with idx {} to sparse with idx {}",
                                                   DataType::TypeToString<SourceIndiceType>(),
                                                   DataType::TypeToString<TargetIndiceType>());
                UnrecoverableError(error_message);
            }
        }
        if constexpr (std::is_same_v<TargetIndiceType, SourceIndiceType>) {
            SparseVecRef<TargetValueType, TargetIndiceType> target_sparse_ref{static_cast<i32>(source_nnz),
                                                                              source_sparse_ref.indices_,
                                                                              target_value_tmp_ptr.get()};
            target.file_offset_ = tgt_vec_buffer->AppendSparse(target_sparse_ref);
        } else {
            auto target_indice_tmp_ptr = MakeUniqueForOverwrite<TargetIndiceType[]>(source_nnz);
            if (!EmbeddingTryCastToFixlen::Run(source_sparse_ref.indices_, target_indice_tmp_ptr.get(), source_nnz)) {
                String error_message = fmt::format("Fail to case from sparse with idx {} to sparse with idx {}",
                                                   DataType::TypeToString<SourceIndiceType>(),
                                                   DataType::TypeToString<TargetIndiceType>());
                UnrecoverableError(error_message);
            }
            SparseVecRef<TargetValueType, TargetIndiceType> target_sparse_ref{static_cast<i32>(source_nnz),
                                                                              target_indice_tmp_ptr.get(),
                                                                              target_value_tmp_ptr.get()};
            target.file_offset_ = tgt_vec_buffer->AppendSparse(target_sparse_ref);
        }
    }
}

template <typename TargetValueType, typename TargetIndiceType, typename SourceValueType, typename SourceIndiceType>
void SparseTryCastToSparseFunT4(const SparseInfo *source_info,
                                const SparseT &source,
                                ColumnVector *source_vector_ptr,
                                const SparseInfo *target_info,
                                SparseT &target,
                                ColumnVector *target_vector_ptr) {
    SparseTryCastToSparseFunInner<TargetValueType, TargetIndiceType, SourceValueType, SourceIndiceType>(source_info,
                                                                                                        source,
                                                                                                        source_vector_ptr->buffer_.get(),
                                                                                                        target_info,
                                                                                                        target,
                                                                                                        target_vector_ptr->buffer_.get());
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
            String error_message = "Invalid source index type";
            UnrecoverableError(error_message);
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
        case kElemBit: {
            String error_message = "Unimplemented";
            UnrecoverableError(error_message);
        }
        case kElemUInt8: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, u8>(source_info,
                                                                              source,
                                                                              source_vector_ptr,
                                                                              target_info,
                                                                              target,
                                                                              target_vector_ptr);
            break;
        }
        case kElemInt8: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, TinyIntT>(source_info,
                                                                                    source,
                                                                                    source_vector_ptr,
                                                                                    target_info,
                                                                                    target,
                                                                                    target_vector_ptr);
            break;
        }
        case kElemInt16: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, SmallIntT>(source_info,
                                                                                     source,
                                                                                     source_vector_ptr,
                                                                                     target_info,
                                                                                     target,
                                                                                     target_vector_ptr);
            break;
        }
        case kElemInt32: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, IntegerT>(source_info,
                                                                                    source,
                                                                                    source_vector_ptr,
                                                                                    target_info,
                                                                                    target,
                                                                                    target_vector_ptr);
            break;
        }
        case kElemInt64: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, BigIntT>(source_info,
                                                                                   source,
                                                                                   source_vector_ptr,
                                                                                   target_info,
                                                                                   target,
                                                                                   target_vector_ptr);
            break;
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
        case kElemFloat16: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, Float16T>(source_info,
                                                                                    source,
                                                                                    source_vector_ptr,
                                                                                    target_info,
                                                                                    target,
                                                                                    target_vector_ptr);
            break;
        }
        case kElemBFloat16: {
            SparseTryCastToSparseFunT3<TargetValueType, TargetIndiceType, BFloat16T>(source_info,
                                                                                     source,
                                                                                     source_vector_ptr,
                                                                                     target_info,
                                                                                     target,
                                                                                     target_vector_ptr);
            break;
        }
        default: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
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
            String error_message = "Invalid target index type";
            UnrecoverableError(error_message);
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
        case kElemBit: {
            SparseTryCastToSparseFunT1<BooleanT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemUInt8: {
            SparseTryCastToSparseFunT1<u8>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemInt8: {
            SparseTryCastToSparseFunT1<TinyIntT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemInt16: {
            SparseTryCastToSparseFunT1<SmallIntT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemInt32: {
            SparseTryCastToSparseFunT1<IntegerT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemInt64: {
            SparseTryCastToSparseFunT1<BigIntT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemFloat: {
            SparseTryCastToSparseFunT1<FloatT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemDouble: {
            SparseTryCastToSparseFunT1<DoubleT>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemFloat16: {
            SparseTryCastToSparseFunT1<Float16T>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        case kElemBFloat16: {
            SparseTryCastToSparseFunT1<BFloat16T>(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
            break;
        }
        default: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
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
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kVarBuffer) {
        String error_message = fmt::format("Sparse column vector should use kHeap VectorBuffer.");
        UnrecoverableError(error_message);
    }
    SparseTryCastToSparseFun(source_info, source, source_vector_ptr, target_info, target, target_vector_ptr);
    return true;
}

} // namespace infinity