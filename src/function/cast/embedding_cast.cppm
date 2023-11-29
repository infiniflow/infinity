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
            Error<TypeException>(Format("Can't cast from Embedding type to {}", target.ToString()));
        }
    }
    return BoundCastFunc(nullptr);
}

struct EmbeddingTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source,
                           const DataType &source_type,
                           TargetType &target,
                           const DataType &target_type,
                           const SharedPtr<ColumnVector> &vector_ptr) {
        Error<FunctionException>(
            Format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
    }
};

template <>
inline bool EmbeddingTryCastToVarlen::Run(const EmbeddingT &source,
                                          const DataType &source_type,
                                          VarcharT &target,
                                          const DataType &target_type,
                                          const SharedPtr<ColumnVector> &vector_ptr) {
    Assert<TypeException>(source_type.type() == LogicalType::kEmbedding,
                          Format("Type here is expected as Embedding, but actually it is: {}", source_type.ToString()));

    EmbeddingInfo *embedding_info = (EmbeddingInfo *)(source_type.type_info().get());

    for (i64 j = 0; j < embedding_info->Dimension(); ++j) {
        LOG_TRACE(Format("{}", ((float *)(source.ptr))[j]));
    }

    String res = EmbeddingT::Embedding2String(source, embedding_info->Type(), embedding_info->Dimension());
    target.length_ = static_cast<u64>(res.size());
    target.is_value_ = false;
    if (target.length_ <= VARCHAR_PREFIX_LEN) {
        // inline varchar
        Memcpy(target.short_.data_, res.c_str(), target.length_);
    } else {
        Assert<TypeException>(vector_ptr->buffer_->buffer_type_ == VectorBufferType::kHeap,
                              "Varchar column vector should use MemoryVectorBuffer.");

        // Set varchar prefix
        Memcpy(target.vector_.prefix_, res.c_str(), VARCHAR_PREFIX_LEN);

        auto [chunk_id, chunk_offset] = vector_ptr->buffer_->fix_heap_mgr_->AppendToHeap(res.c_str(), target.length_);
        target.vector_.chunk_id_ = chunk_id;
        target.vector_.chunk_offset_ = chunk_offset;
    }

    return true;
}

} // namespace infinity