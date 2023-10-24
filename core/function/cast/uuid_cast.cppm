//
// Created by jinhai on 22-12-24.
//

module;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import parser;
import column_vector_cast;
import infinity_assert;
import infinity_exception;
import third_party;

export module uuid_cast;

namespace infinity {

export struct UuidTryCastToVarlen;

export inline BoundCastFunc BindUuidCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<UuidT, VarcharT, UuidTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Uuid type to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct UuidTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
    }
};

template <>
inline bool UuidTryCastToVarlen::Run(const UuidT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    target.length = UuidT::LENGTH;
    Memcpy(target.prefix, source.body, VarcharT::PREFIX_LENGTH);
    Assert<TypeException>(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                          "Varchar column vector should use MemoryVectorBuffer.",
                          __FILE_NAME__,
                          __LINE__);

    ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
    Memcpy(ptr, source.body, target.length);
    target.ptr = ptr;

    return true;
}

} // namespace infinity
