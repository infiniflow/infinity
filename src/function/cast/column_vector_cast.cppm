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

import column_vector;
import stl;
import parser;
import bitmask;
import bound_cast_func;
import unary_operator;
import null_value;

export module column_vector_cast;

namespace infinity {

struct ColumnVectorCastData {
    explicit ColumnVectorCastData(bool strict, SharedPtr<ColumnVector> &column_vector_ptr, DataType source_type, DataType target_type)
        : strict_(strict), column_vector_ptr_(column_vector_ptr), source_type_(Move(source_type)), target_type_(Move(target_type)) {}

    DataType source_type_{LogicalType::kInvalid};
    DataType target_type_{LogicalType::kInvalid};
    bool strict_{false};
    bool all_converted_{true};
    SharedPtr<ColumnVector> column_vector_ptr_{nullptr};
};

template <typename Operator>
struct TryCastValue {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        if (Operator::template Run<SourceValueType, TargetValueType>(input, result)) {
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
struct TryCastValueToVarlen {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *cast_data_ptr = (ColumnVectorCastData *)(state_ptr);
        if (Operator::template Run<SourceValueType, TargetValueType>(input, result, cast_data_ptr->column_vector_ptr_)) {
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

        auto *data_ptr = (ColumnVectorCastData *)(state_ptr);
        // This convert is failed
        data_ptr->all_converted_ = false;
        //        data_ptr->source_type_  =
        //        data_ptr->target_type_  =
        //        data_ptr->result_ref_
    }
};

template <typename Operator>
struct TryCastValueToVarlenWithType {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *cast_data_ptr = (ColumnVectorCastData *)(state_ptr);
        //        LOG_TRACE("{}, {}", cast_data_ptr->source_type_.ToString(), cast_data_ptr->target_type_.ToString());
        if (Operator::template Run<SourceValueType, TargetValueType>(input,
                                                                     cast_data_ptr->source_type_,
                                                                     result,
                                                                     cast_data_ptr->target_type_,
                                                                     cast_data_ptr->column_vector_ptr_)) {
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

export struct ColumnVectorCast {

    template <class SourceType, class TargetType, class Operator>
    static bool
    GenericTryCastColumnVector(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &result, SizeT count, CastParameters &parameters) {
        ColumnVectorCastData input(parameters.strict, result, *source->data_type_, *result->data_type_);
        UnaryOperator::Execute<SourceType, TargetType, Operator>(source, result, count, &input, true);
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
};

} // namespace infinity