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

export module tensor_array_cast;

import stl;
import column_vector;
import vector_buffer;
import bound_cast_func;
import column_vector_cast;
import float_cast;
import integer_cast;
import infinity_exception;
import third_party;
import logger;
import status;
import logical_type;
import internal_types;
import embedding_info;
import knn_expr;
import data_type;
import default_values;
import embedding_cast;
import tensor_cast;

namespace infinity {

struct TensorArrayTryCastToTensorArray;

export inline BoundCastFunc BindTensorArrayCast(const DataType &source, const DataType &target) {
    // now we only support cast between two tensor-arrays of the same basic dimension
    if (source.type() != LogicalType::kTensorArray || target.type() != LogicalType::kTensorArray) {
        RecoverableError(Status::NotSupportedTypeConversion(source.ToString(), target.ToString()));
    }
    auto source_info = static_cast<const EmbeddingInfo *>(source.type_info().get());
    auto target_info = static_cast<const EmbeddingInfo *>(target.type_info().get());
    if (source_info->Dimension() != target_info->Dimension()) {
        RecoverableError(Status::DataTypeMismatch(source.ToString(), target.ToString()));
    }
    return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorVarlenWithType<TensorArrayT, TensorArrayT, TensorArrayTryCastToTensorArray>);
}

template <typename TargetValueType, typename SourceValueType>
void TensorArrayTryCastToTensorArrayImpl(const u32 basic_embedding_dim,
                                         const TensorArrayT &source,
                                         const ColumnVector *source_vector_ptr,
                                         TensorArrayT &target,
                                         ColumnVector *target_vector_ptr) {
    const Vector<TensorT> &source_tensors = ColumnVector::GetTensorArrayMeta(source, source_vector_ptr->buffer_.get());
    SizeT tensor_num = source_tensors.size();
    Vector<TensorT> target_tensors(tensor_num);
    for (SizeT i = 0; i < tensor_num; ++i) {
        TensorTryCastToTensorImpl<TargetValueType, SourceValueType>(source_tensors.data()[i],
                                                                    source_vector_ptr,
                                                                    target_tensors[i],
                                                                    target_vector_ptr);
    }
    ColumnVector::SetTensorArrayMeta(target, target_vector_ptr->buffer_.get(), {target_tensors.data(), tensor_num});
}

template <typename TargetValueType>
void TensorArrayTryCastToTensorArrayImpl(const u32 basic_embedding_dim,
                                         const TensorArrayT &source,
                                         const EmbeddingDataType src_type,
                                         ColumnVector *source_vector_ptr,
                                         TensorArrayT &target,
                                         ColumnVector *target_vector_ptr) {
    switch (src_type) {
        case EmbeddingDataType::kElemBit: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, BooleanT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, u8>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, TinyIntT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, SmallIntT>(basic_embedding_dim,
                                                                            source,
                                                                            source_vector_ptr,
                                                                            target,
                                                                            target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, IntegerT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, BigIntT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, FloatT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, DoubleT>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, Float16T>(basic_embedding_dim, source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            TensorArrayTryCastToTensorArrayImpl<TargetValueType, BFloat16T>(basic_embedding_dim,
                                                                            source,
                                                                            source_vector_ptr,
                                                                            target,
                                                                            target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
        }
    }
}

void TensorArrayTryCastToTensorArrayFun(const u32 basic_embedding_dim,
                                        const TensorArrayT &source,
                                        const EmbeddingDataType src_type,
                                        ColumnVector *source_vector_ptr,
                                        TensorArrayT &target,
                                        const EmbeddingDataType dst_type,
                                        ColumnVector *target_vector_ptr) {
    switch (dst_type) {
        case EmbeddingDataType::kElemBit: {
            TensorArrayTryCastToTensorArrayImpl<BooleanT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            TensorArrayTryCastToTensorArrayImpl<u8>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            TensorArrayTryCastToTensorArrayImpl<TinyIntT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            TensorArrayTryCastToTensorArrayImpl<SmallIntT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            TensorArrayTryCastToTensorArrayImpl<IntegerT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            TensorArrayTryCastToTensorArrayImpl<BigIntT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            TensorArrayTryCastToTensorArrayImpl<FloatT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            TensorArrayTryCastToTensorArrayImpl<DoubleT>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            TensorArrayTryCastToTensorArrayImpl<Float16T>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            TensorArrayTryCastToTensorArrayImpl<BFloat16T>(basic_embedding_dim, source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
        }
    }
}

struct TensorArrayTryCastToTensorArray {
    template <typename SourceT, typename TargetT>
    static bool Run(const SourceT &source,
                    const DataType &source_type,
                    ColumnVector *source_vector_ptr,
                    TargetT &target,
                    const DataType &target_type,
                    ColumnVector *target_vector_ptr) {
        String error_message = "Unreachable case";
        UnrecoverableError(error_message);
        return false;
    }
};

template <>
bool TensorArrayTryCastToTensorArray::Run<TensorArrayT, TensorArrayT>(const TensorArrayT &source,
                                                                      const DataType &source_type,
                                                                      ColumnVector *source_vector_ptr,
                                                                      TensorArrayT &target,
                                                                      const DataType &target_type,
                                                                      ColumnVector *target_vector_ptr) {
    const EmbeddingInfo *source_embedding_info = (EmbeddingInfo *)(source_type.type_info().get());
    const EmbeddingInfo *target_embedding_info = (EmbeddingInfo *)(target_type.type_info().get());
    const auto source_embedding_dim = source_embedding_info->Dimension();
    const auto target_embedding_dim = target_embedding_info->Dimension();
    if (source_embedding_dim != target_embedding_dim) {
        RecoverableError(Status::DataTypeMismatch(source_type.ToString(), target_type.ToString()));
    }
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kVarBuffer) {
        String error_message = fmt::format("TensorArray column vector should use kHeap VectorBuffer.");
        UnrecoverableError(error_message);
    }
    TensorArrayTryCastToTensorArrayFun(source_embedding_dim,
                                       source,
                                       source_embedding_info->Type(),
                                       source_vector_ptr,
                                       target,
                                       target_embedding_info->Type(),
                                       target_vector_ptr);
    return true;
}

} // namespace infinity
