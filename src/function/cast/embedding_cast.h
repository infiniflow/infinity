//
// Created by jinhai on 22-12-24.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct EmbeddingTryCastToVarlen;

inline static BoundCastFunc BindEmbeddingCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlenWithType<EmbeddingT, VarcharT, EmbeddingTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Time type to " + target.ToString());
        }
    }
}

struct EmbeddingTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source,
                           const DataType &source_type,
                           TargetType &target,
                           const DataType &target_type,
                           const SharedPtr<ColumnVector> &vector_ptr) {
        FunctionError("Not support to cast from " + source_type.ToString() + " to " + target_type.ToString());
    }
};

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          VarcharT &target,
                                          const DataType &target_type,
                                          const SharedPtr<ColumnVector> &vector_ptr) {
    LOG_TRACE("{}", "Not support to cast from " + source_type.ToString() + " to " + target_type.ToString());

    TypeAssert(source_type.type() == LogicalType::kEmbedding, "Type here is expected as Embedding, but actually it is: " + source_type.ToString());

    SharedPtr<EmbeddingInfo> embedding_info = std::static_pointer_cast<EmbeddingInfo>(source_type.type_info());

    for (i64 j = 0; j < embedding_info->Dimension(); ++j) {
        LOG_TRACE(((float *)(source.ptr))[j]);
    }

    String res = EmbeddingT::Embedding2String(source, embedding_info->Type(), embedding_info->Dimension());
    target.length = static_cast<u16>(res.size());
    if (target.length <= VarcharT::INLINE_LENGTH) {
        // inline varchar
        memcpy(target.prefix, source.ptr, target.length);
        memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        TypeAssert(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap, "Varchar column vector should use MemoryVectorBuffer. ");
        // Set varchar prefix
        memcpy(target.prefix, source.ptr, VarcharT::PREFIX_LENGTH);

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        memcpy(ptr, source.ptr, target.length);
        target.ptr = ptr;
    }

    return true;
}

} // namespace infinity