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

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import parser;
import column_vector_cast;
import float_cast;
import integer_cast;
import infinity_exception;
import third_party;
import logger;

export module embedding_cast;

namespace infinity {

struct EmbeddingTryCastToFixlen;
struct EmbeddingTryCastToVarlen;

template <typename SourceElemType>
BoundCastFunc BindEmbeddingCast(const EmbeddingInfo *target);

export inline BoundCastFunc BindEmbeddingCast(const DataType &source, const DataType &target) {
    if (source.type() != LogicalType::kEmbedding || target.type() != LogicalType::kEmbedding) {
        Error<TypeException>(fmt::format("Type here is expected as Embedding, but actually it is: {} and {}", source.ToString(), target.ToString()));
    }
    auto source_info = static_cast<const EmbeddingInfo *>(source.type_info().get());
    auto target_info = static_cast<const EmbeddingInfo *>(target.type_info().get());
    if (source_info->Dimension() != target_info->Dimension()) {
        Error<TypeException>(fmt::format("Can't cast from Embedding type to {}", target.ToString()));
    }
    switch (source_info->Type()) {
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
        default: {
            Error<TypeException>(fmt::format("Can't cast from {} to Embedding type", target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

template <typename SourceElemType>
inline BoundCastFunc BindEmbeddingCast(const EmbeddingInfo *target) {
    switch (target->Type()) {
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
        default: {
            Error<TypeException>(fmt::format("Can't cast from Embedding type to {}", target->ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct EmbeddingTryCastToFixlen {
    template <typename SourceElemType, typename TargetElemType>
    static inline bool Run(const SourceElemType *source, TargetElemType *target, SizeT len) {
        if constexpr (IsSame<SourceElemType, TinyIntT>() || IsSame<SourceElemType, SmallIntT>() || IsSame<SourceElemType, IntegerT>() ||
                      IsSame<SourceElemType, BigIntT>()) {
            for (SizeT i = 0; i < len; ++i) {
                if (!IntegerTryCastToFixlen::Run(source[i], target[i])) {
                    return false;
                }
            }
            return true;
        } else if constexpr (IsSame<SourceElemType, FloatT>() || IsSame<SourceElemType, DoubleT>()) {
            for (SizeT i = 0; i < len; ++i) {
                if (!FloatTryCastToFixlen::Run(source[i], target[i])) {
                    return false;
                }
            }
            return true;
        }
        Error<FunctionException>(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceElemType>(), DataType::TypeToString<TargetElemType>()));
    }
};

struct EmbeddingTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &,
                           const DataType &,
                           TargetType &,
                           const DataType &,
                           const SharedPtr<ColumnVector> &) {
        Error<FunctionException>(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          VarcharT &target,
                                          const DataType &,
                                          const SharedPtr<ColumnVector> &vector_ptr) {
    if (source_type.type() != LogicalType::kEmbedding) {
        Error<TypeException>(fmt::format("Type here is expected as Embedding, but actually it is: {}", source_type.ToString()));
    }

    EmbeddingInfo *embedding_info = (EmbeddingInfo *)(source_type.type_info().get());

    for (SizeT j = 0; j < embedding_info->Dimension(); ++j) {
        LOG_TRACE(fmt::format("{}", ((float *)(source.ptr))[j]));
    }

    String res = EmbeddingT::Embedding2String(source, embedding_info->Type(), embedding_info->Dimension());
    target.length_ = static_cast<u64>(res.size());
    target.is_value_ = false;
    if (target.length_ <= VARCHAR_PREFIX_LEN) {
        // inline varchar
        Memcpy(target.short_.data_, res.c_str(), target.length_);
    } else {
        if (vector_ptr->buffer_->buffer_type_ != VectorBufferType::kHeap) {
            Error<TypeException>(fmt::format("Varchar column vector should use MemoryVectorBuffer."));
        }

        // Set varchar prefix
        Memcpy(target.vector_.prefix_, res.c_str(), VARCHAR_PREFIX_LEN);

        auto [chunk_id, chunk_offset] = vector_ptr->buffer_->fix_heap_mgr_->AppendToHeap(res.c_str(), target.length_);
        target.vector_.chunk_id_ = chunk_id;
        target.vector_.chunk_offset_ = chunk_offset;
    }

    return true;
}

} // namespace infinity