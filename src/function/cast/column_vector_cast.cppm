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

export module column_vector_cast;

import column_vector;
import stl;

import roaring_bitmap;
import bound_cast_func;
import unary_operator;
import embedding_unary_operator;
import null_value;
import logical_type;
import internal_types;
import data_type;

namespace infinity {

struct ColumnVectorCastData {
    explicit ColumnVectorCastData(bool strict,
                                  ColumnVector *source_column_vector_ptr,
                                  ColumnVector *target_column_vector_ptr,
                                  DataType source_type,
                                  DataType target_type)
        : source_type_(std::move(source_type)), target_type_(std::move(target_type)), strict_(strict),
          source_column_vector_ptr_(source_column_vector_ptr), target_column_vector_ptr_(target_column_vector_ptr) {}

    DataType source_type_{LogicalType::kInvalid};
    DataType target_type_{LogicalType::kInvalid};
    bool strict_{false};
    bool all_converted_{true};
    ColumnVector *source_column_vector_ptr_{nullptr};
    ColumnVector *target_column_vector_ptr_{nullptr};
};

template <typename Operator>
struct TryCastValue {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void*, void *state_ptr) {
        if (Operator::template Run<SourceValueType, TargetValueType>(std::move(input), result)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();

        auto *data_ptr = (ColumnVectorCastData *)(state_ptr);
        // This convert is failed
        data_ptr->all_converted_ = false;
        //        data_ptr->source_type_  =
        //        data_ptr->target_type_  =
        //        data_ptr->result_ref_
    }
};

template <typename Operator>
struct TryCastVarlenToValue {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void*, void *state_ptr) {
        auto *cast_data_ptr = (ColumnVectorCastData *)(state_ptr);

        if (Operator::template Run<SourceValueType, TargetValueType>(std::move(input), cast_data_ptr->source_column_vector_ptr_, result)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
        // This convert is failed
        cast_data_ptr->all_converted_ = false;
        //        data_ptr->source_type_  =
        //        data_ptr->target_type_  =
        //        data_ptr->result_ref_
    }
};

template <typename Operator>
struct TryCastValueToVarlen {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void*, void *state_ptr) {
        auto *cast_data_ptr = (ColumnVectorCastData *)(state_ptr);
        if (Operator::template Run<SourceValueType, TargetValueType>(input, result, cast_data_ptr->target_column_vector_ptr_)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();

        // This convert is failed
        cast_data_ptr->all_converted_ = false;
        //        data_ptr->source_type_  =
        //        data_ptr->target_type_  =
        //        data_ptr->result_ref_
    }
};

template <typename Operator>
struct TryCastValueWithType {
    // Now, this class is used to cast value to CharT
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *cast_data_ptr = (ColumnVectorCastData *)(state_ptr);
        //        LOG_TRACE("{}, {}", cast_data_ptr->source_type_.ToString(), cast_data_ptr->target_type_.ToString());
        if (Operator::template Run<SourceValueType, TargetValueType>(input, cast_data_ptr->source_type_, result, cast_data_ptr->target_type_)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();

        // This convert is failed
        cast_data_ptr->all_converted_ = false;
        //        data_ptr->source_type_  =
        //        data_ptr->target_type_  =
        //        data_ptr->result_ref_
    }
};

template <typename Operator>
struct TryCastValueToVarlenWithType {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void*, void *state_ptr) {
        auto *cast_data_ptr = (ColumnVectorCastData *)(state_ptr);
        // LOG_INFO(fmt::format("{}, {}", cast_data_ptr->source_type_.ToString(), cast_data_ptr->target_type_.ToString()));
        if (Operator::template Run<SourceValueType, TargetValueType>(input,
                                                                     cast_data_ptr->source_type_,
                                                                     result,
                                                                     cast_data_ptr->target_type_,
                                                                     cast_data_ptr->target_column_vector_ptr_)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();

        // This convert is failed
        cast_data_ptr->all_converted_ = false;
        //        data_ptr->source_type_  =
        //        data_ptr->target_type_  =
        //        data_ptr->result_ref_
    }
};

template <typename Operator>
struct TryCastVarlenWithTypeToVarlenWithType {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void*, void *state_ptr) {
        auto *cast_data_ptr = (ColumnVectorCastData *)(state_ptr);
        // LOG_INFO(fmt::format("{}, {}", cast_data_ptr->source_type_.ToString(), cast_data_ptr->target_type_.ToString()));
        if (Operator::template Run<SourceValueType, TargetValueType>(input,
                                                                     cast_data_ptr->source_type_,
                                                                     cast_data_ptr->source_column_vector_ptr_,
                                                                     result,
                                                                     cast_data_ptr->target_type_,
                                                                     cast_data_ptr->target_column_vector_ptr_)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();

        // This convert is failed
        cast_data_ptr->all_converted_ = false;
        //        data_ptr->source_type_  =
        //        data_ptr->target_type_  =
        //        data_ptr->result_ref_
    }
};

template <typename Operator>
struct TryCastValueEmbedding {
    template <typename SourceElemType, typename TargetElemType>
    inline static void Execute(const SourceElemType *input, TargetElemType *result, SizeT dim, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        if (Operator::template Run<SourceElemType, TargetElemType>(input, result, dim)) {
            return;
        }

        nulls_ptr->SetFalse(idx);

        SetEmbeddingNullValue(result, dim);

        auto *data_ptr = (ColumnVectorCastData *)(state_ptr);
        // This convert is failed
        data_ptr->all_converted_ = false;
    }
};

export struct ColumnVectorCast {

    template <class SourceType, class TargetType, class Operator>
    static bool
    GenericTryCastColumnVector(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &result, SizeT count, CastParameters &parameters) {
        ColumnVectorCastData input(parameters.strict, source.get(), result.get(), *source->data_type(), *result->data_type());
        UnaryOperator::Execute<SourceType, TargetType, Operator>(source, result, count, nullptr, &input, true);
        return input.all_converted_;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool
    TryCastColumnVector(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, SizeT count, CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValue<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool
    TryCastVarlenColumnVector(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, SizeT count, CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastVarlenToValue<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool
    TryCastColumnVectorToVarlen(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, SizeT count, CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValueToVarlen<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool TryCastColumnVectorToVarlenWithType(const SharedPtr<ColumnVector> &source,
                                                           SharedPtr<ColumnVector> &target,
                                                           SizeT count,
                                                           CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValueToVarlenWithType<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool
    TryCastColumnVectorWithType(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, SizeT count, CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValueWithType<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceElemType, class TargetElemType, class Operator>
    inline static bool
    TryCastColumnVectorEmbedding(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, SizeT count, CastParameters &parameters) {
        ColumnVectorCastData input(parameters.strict, source.get(), target.get(), *source->data_type(), *target->data_type());
        EmbeddingUnaryOperator::Execute<SourceElemType, TargetElemType, TryCastValueEmbedding<Operator>>(source, target, count, &input, true);
        return input.all_converted_;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool TryCastColumnVectorVarlenWithType(const SharedPtr<ColumnVector> &source,
                                                         SharedPtr<ColumnVector> &target,
                                                         SizeT count,
                                                         CastParameters &parameters) {
        bool result =
            GenericTryCastColumnVector<SourceType, TargetType, TryCastVarlenWithTypeToVarlenWithType<Operator>>(source, target, count, parameters);
        return result;
    }
};

} // namespace infinity