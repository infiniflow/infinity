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

export module tensor_cast;

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
import fix_heap;

namespace infinity {

struct TensorTryCastToTensor;

export inline BoundCastFunc BindTensorCast(const DataType &source, const DataType &target) {
    // now we only support cast between two tensors of the same basic dimension
    if (source.type() != LogicalType::kTensor || target.type() != LogicalType::kTensor) {
        RecoverableError(Status::NotSupportedTypeConversion(source.ToString(), target.ToString()));
    }
    auto source_info = static_cast<const EmbeddingInfo *>(source.type_info().get());
    auto target_info = static_cast<const EmbeddingInfo *>(target.type_info().get());
    if (source_info->Dimension() != target_info->Dimension()) {
        RecoverableError(Status::DataTypeMismatch(source.ToString(), target.ToString()));
    }
    return BoundCastFunc(&ColumnVectorCast::TryCastColumnVectorVarlenWithType<TensorT, TensorT, TensorTryCastToTensor>);
}

export template <typename TargetValueType, typename SourceValueType>
void TensorTryCastToTensorImpl(const TensorT &source, const ColumnVector *source_vector_ptr, TensorT &target, ColumnVector *target_vector_ptr) {
    const auto *source_embedding_info = static_cast<const EmbeddingInfo *>(source_vector_ptr->data_type()->type_info().get());
    const auto *target_embedding_info = static_cast<const EmbeddingInfo *>(target_vector_ptr->data_type()->type_info().get());
    if (source_embedding_info->Dimension() != target_embedding_info->Dimension()) {
        RecoverableError(Status::DataTypeMismatch(source_vector_ptr->data_type()->ToString(), target_vector_ptr->data_type()->ToString()));
    }

    auto [raw_data, embedding_num] = ColumnVector::GetTensor(source, source_vector_ptr->buffer_.get(), source_embedding_info);
    SizeT source_total_dim = source_embedding_info->Dimension() * embedding_num;
    if constexpr (std::is_same_v<TargetValueType, SourceValueType>) {
        ColumnVector::SetTensor(target, target_vector_ptr->buffer_.get(), raw_data, target_embedding_info);
    } else if constexpr (std::is_same_v<TargetValueType, BooleanT>) {
        static_assert(sizeof(bool) == 1);
        const auto target_size = (source_total_dim + 7) / 8;
        auto target_tmp_ptr = MakeUnique<char[]>(target_size);
        auto src_ptr = reinterpret_cast<const SourceValueType *>(raw_data.data());
        for (SizeT i = 0; i < source_total_dim; ++i) {
            if (src_ptr[i]) {
                target_tmp_ptr[i / 8] |= (1u << (i % 8));
            }
        }
        ColumnVector::SetTensor(target, target_vector_ptr->buffer_.get(), {target_tmp_ptr.get(), target_size}, target_embedding_info);
    } else {
        const auto target_size = source_total_dim * sizeof(TargetValueType);
        auto target_tmp_ptr = MakeUniqueForOverwrite<TargetValueType[]>(source_total_dim);
        if (!EmbeddingTryCastToFixlen::Run(reinterpret_cast<const SourceValueType *>(raw_data.data()),
                                           reinterpret_cast<TargetValueType *>(target_tmp_ptr.get()),
                                           source_total_dim)) {
            String error_message = fmt::format("Failed to cast from tensor with type {} to tensor with type {}",
                                               DataType::TypeToString<SourceValueType>(),
                                               DataType::TypeToString<TargetValueType>());
            UnrecoverableError(error_message);
        }
        ColumnVector::SetTensor(target,
                                target_vector_ptr->buffer_.get(),
                                {reinterpret_cast<const char *>(target_tmp_ptr.get()), target_size},
                                target_embedding_info);
    }
}

template <typename TargetValueType>
void TensorTryCastToTensorImpl(const TensorT &source,
                               const EmbeddingDataType src_type,
                               const ColumnVector *source_vector_ptr,
                               TensorT &target,
                               ColumnVector *target_vector_ptr) {
    switch (src_type) {
        case EmbeddingDataType::kElemBit: {
            TensorTryCastToTensorImpl<TargetValueType, BooleanT>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            TensorTryCastToTensorImpl<TargetValueType, u8>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            TensorTryCastToTensorImpl<TargetValueType, TinyIntT>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            TensorTryCastToTensorImpl<TargetValueType, SmallIntT>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            TensorTryCastToTensorImpl<TargetValueType, IntegerT>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            TensorTryCastToTensorImpl<TargetValueType, BigIntT>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            TensorTryCastToTensorImpl<TargetValueType, FloatT>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            TensorTryCastToTensorImpl<TargetValueType, DoubleT>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            TensorTryCastToTensorImpl<TargetValueType, Float16T>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            TensorTryCastToTensorImpl<TargetValueType, BFloat16T>(source, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
        }
    }
}

void TensorTryCastToTensorFun(const TensorT &source,
                              const EmbeddingDataType src_type,
                              const ColumnVector *source_vector_ptr,
                              TensorT &target,
                              const EmbeddingDataType dst_type,
                              ColumnVector *target_vector_ptr) {
    switch (dst_type) {
        case EmbeddingDataType::kElemBit: {
            TensorTryCastToTensorImpl<BooleanT>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            TensorTryCastToTensorImpl<u8>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            TensorTryCastToTensorImpl<TinyIntT>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            TensorTryCastToTensorImpl<SmallIntT>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            TensorTryCastToTensorImpl<IntegerT>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            TensorTryCastToTensorImpl<BigIntT>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            TensorTryCastToTensorImpl<FloatT>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            TensorTryCastToTensorImpl<DoubleT>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            TensorTryCastToTensorImpl<Float16T>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            TensorTryCastToTensorImpl<BFloat16T>(source, src_type, source_vector_ptr, target, target_vector_ptr);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            String error_message = "Unreachable code";
            UnrecoverableError(error_message);
        }
    }
}

struct TensorTryCastToTensor {
    template <typename SourceT, typename TargetT>
    static bool Run(const SourceT &source,
                    const DataType &source_type,
                    const ColumnVector *source_vector_ptr,
                    TargetT &target,
                    const DataType &target_type,
                    ColumnVector *target_vector_ptr) {
        String error_message = "Unreachable case";
        UnrecoverableError(error_message);
        return false;
    }
};

template <>
bool TensorTryCastToTensor::Run<TensorT, TensorT>(const TensorT &source,
                                                  const DataType &source_type,
                                                  const ColumnVector *source_vector_ptr,
                                                  TensorT &target,
                                                  const DataType &target_type,
                                                  ColumnVector *target_vector_ptr) {
    const EmbeddingInfo *source_embedding_info = (EmbeddingInfo *)(source_type.type_info().get());
    const EmbeddingInfo *target_embedding_info = (EmbeddingInfo *)(target_type.type_info().get());
    if (target_vector_ptr->buffer_->buffer_type_ != VectorBufferType::kVarBuffer) {
        String error_message = fmt::format("Tensor column vector should use kVarBuffer VectorBuffer.");
        UnrecoverableError(error_message);
    }
    TensorTryCastToTensorFun(source, source_embedding_info->Type(), source_vector_ptr, target, target_embedding_info->Type(), target_vector_ptr);
    return true;
}

} // namespace infinity
