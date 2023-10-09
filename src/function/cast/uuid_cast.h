//
// Created by jinhai on 22-12-24.
//

#pragma once

#include "bound_cast_func.h"
#include "column_vector_cast.h"
#include "common/types/data_type.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

struct UuidTryCastToVarlen;

inline static BoundCastFunc BindUuidCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<UuidT, VarcharT, UuidTryCastToVarlen>);
        }
        default: {
            TypeError("Can't cast from Time type to " + target.ToString());
        }
    }
}

struct UuidTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        FunctionError("Not support to cast from " + DataType::TypeToString<SourceType>() + " to " + DataType::TypeToString<TargetType>());
    }
};

template <>
inline bool UuidTryCastToVarlen::Run(const UuidT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    target.length = UuidT::LENGTH;
    memcpy(target.prefix, source.body, VarcharT::PREFIX_LENGTH);
    TypeAssert(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap, "Varchar column vector should use MemoryVectorBuffer. ");

    ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
    memcpy(ptr, source.body, target.length);
    target.ptr = ptr;

    return true;
}

} // namespace infinity
