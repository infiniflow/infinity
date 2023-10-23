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

export module blob_cast;

namespace infinity {

export struct BlobTryCastToVarlen;

export inline BoundCastFunc BindBlobCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<BlobT, VarcharT, BlobTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Blob type to {}", target.ToString()), __FILE_NAME__, __LINE__);
        }
    }
}

struct BlobTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
    }
};

template <>
inline bool BlobTryCastToVarlen::Run(const BlobT &source, VarcharT &target, const SharedPtr<ColumnVector> &vector_ptr) {
    if (source.size > VarcharT::LEN_LIMIT) {
        return false;
    }

    target.length = static_cast<u16>(source.size);
    if (target.length <= VarcharT::INLINE_LENGTH) {
        // inline varchar
        Memcpy(target.prefix, source.ptr, target.length);
        Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        Assert<FunctionException>(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                                  "Varchar column vector should use MemoryVectorBuffer. ",
                                  __FILE_NAME__,
                                  __LINE__);
        // Set varchar prefix
        Memcpy(target.prefix, source.ptr, VarcharT::PREFIX_LENGTH);

        ptr_t ptr = vector_ptr->buffer_->heap_mgr_->Allocate(target.length);
        Memcpy(ptr, source.ptr, target.length);
        target.ptr = ptr;
    }

    return true;
}

} // namespace infinity
