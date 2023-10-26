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
import logger;

export module embedding_cast;

namespace infinity {

struct EmbeddingTryCastToVarlen;

export inline BoundCastFunc BindEmbeddingCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlenWithType<EmbeddingT, VarcharT, EmbeddingTryCastToVarlen>);
        }
        default: {
            Error<TypeException>(Format("Can't cast from Embedding type to {}", target.ToString()), __FILE_NAME__, __LINE__);
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
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()),
            __FILE_NAME__,
            __LINE__);
    }
};

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          VarcharT &target,
                                          const DataType &target_type,
                                          const SharedPtr<ColumnVector> &vector_ptr) {
    Assert<TypeException>(source_type.type() == LogicalType::kEmbedding,
                          Format("Type here is expected as Embedding, but actually it is: {}", source_type.ToString()),
                          __FILE_NAME__,
                          __LINE__);

    EmbeddingInfo *embedding_info = (EmbeddingInfo *)(source_type.type_info().get());

    for (i64 j = 0; j < embedding_info->Dimension(); ++j) {
        LOG_TRACE(Format("{}", ((float *)(source.ptr))[j]));
    }

    String res = EmbeddingT::Embedding2String(source, embedding_info->Type(), embedding_info->Dimension());
    target.length = static_cast<u16>(res.size());
    if (target.length <= VarcharT::INLINE_LENGTH) {
        // inline varchar
        Memcpy(target.prefix, source.ptr, target.length);
        Memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        Assert<TypeException>(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                              "Varchar column vector should use MemoryVectorBuffer.",
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