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

import column_vector_cast;

import infinity_exception;
import third_party;

export module blob_cast;
#if 0
namespace infinity {

export struct BlobTryCastToVarlen;

export inline BoundCastFunc BindBlobCast(DataType &target) {
    switch (target.type()) {
        case LogicalType::kVarchar: {
            return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorToVarlen<BlobT, VarcharT, BlobTryCastToVarlen>);
        }
        default: {
            String error_message = fmt::format("Can't cast from Blob type to {}", target.ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
    return BoundCastFunc(nullptr);
}

struct BlobTryCastToVarlen {
    template <typename SourceType, typename TargetType>
    static inline bool Run(const SourceType &source, TargetType &target, const SharedPtr<ColumnVector> &vector_ptr) {
        UnrecoverableError(
            fmt::format("Not support to cast from {} to {}", DataType::TypeToString<SourceType>(), DataType::TypeToString<TargetType>()));
        return false;
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
        std::memcpy(target.prefix, source.ptr, target.length);
        std::memset(target.prefix + target.length, 0, VarcharT::INLINE_LENGTH - target.length);
    } else {
        if(vector_ptr->buffer_->buffer_type_ != VectorBufferType::kHeap) {
            String error_message = "Simple Aggregate without aggregate expression.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }

        // Set varchar prefix
        std::memcpy(target.prefix, source.ptr, VarcharT::PREFIX_LENGTH);

        ptr_t ptr = vector_ptr->buffer_->fix_heap_mgr_->Allocate(target.length);
        std::memcpy(ptr, source.ptr, target.length);
        target.ptr = ptr;
    }

    return true;
}

} // namespace infinity
#endif