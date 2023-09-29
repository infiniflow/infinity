//
// Created by jinhai on 22-12-24.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct BlobTryCastToVarlen;

inline static BoundCastFunc
BindBlobCast(DataType& target) {
    switch(target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<BlobT, VarcharT, BlobTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Time type to " + target.ToString());
        }
    }
}

struct BlobTryCastToVarlen {
    template<typename SourceType, typename TargetType>
    static inline bool
    Run(const SourceType& source, TargetType& target, const SharedPtr<ColumnVector>& vector_ptr) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>()
                      + " to " + DataType::TypeToString<TargetType>());
    }
};

template<>
inline bool
BlobTryCastToVarlen::Run(const BlobT& source, VarcharT& target, const SharedPtr<ColumnVector>& vector_ptr) {
    if(source.size > VarcharT::LEN_LIMIT) {
        return false;
    }

    target.length = static_cast<u16>(source.size);
    if(target.length <= VarcharT::INLINE_LENGTH) {
        // inline varchar
        memcpy(target.prefix, source.ptr, target.length);
        memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        TypeAssert(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                   "Varchar column vector should use MemoryVectorBuffer. ");
        // Set varchar prefix
        memcpy(target.prefix, source.ptr, VarcharT::PREFIX_LENGTH);

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        memcpy(ptr, source.ptr, target.length);
        target.ptr = ptr;
    }

    return true;
}

}
