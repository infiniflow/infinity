//
// Created by JinHai on 2022/12/15.
//

#pragma once

#include "common/column_vector/operator/unary_operator.h"
#include "common/types/null_value.h"
#include "common/column_vector/bitmask.h"

namespace infinity {

struct ColumnVectorCastData {
    explicit ColumnVectorCastData(bool strict, SharedPtr<ColumnVector> &column_vector_ptr, DataType source_type, DataType target_type)
        : strict_(strict), column_vector_ptr_(column_vector_ptr), source_type_(std::move(source_type)), target_type_(std::move(target_type)) {}

    DataType source_type_{LogicalType::kInvalid};
    DataType target_type_{LogicalType::kInvalid};
    bool strict_{false};
    bool all_converted_{true};
    SharedPtr<ColumnVector> column_vector_ptr_{nullptr};
};

template <typename Operator>
struct TryCastValue {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
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
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
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
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
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
    inline static void Execute(const SourceValueType &input, TargetValueType &result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
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

struct ColumnVectorCast {

    template <class SourceType, class TargetType, class Operator>
    static bool
    GenericTryCastColumnVector(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &result, size_t count, CastParameters &parameters) {
        ColumnVectorCastData input(parameters.strict, result, *source->data_type_, *result->data_type_);
        UnaryOperator::Execute<SourceType, TargetType, Operator>(source, result, count, &input, true);
        return input.all_converted_;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool
    TryCastColumnVector(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, size_t count, CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValue<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool
    TryCastColumnVectorToVarlen(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, size_t count, CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValueToVarlen<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool TryCastColumnVectorToVarlenWithType(const SharedPtr<ColumnVector> &source,
                                                           SharedPtr<ColumnVector> &target,
                                                           size_t count,
                                                           CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValueToVarlenWithType<Operator>>(source, target, count, parameters);
        return result;
    }

    template <class SourceType, class TargetType, class Operator>
    inline static bool
    TryCastColumnVectorWithType(const SharedPtr<ColumnVector> &source, SharedPtr<ColumnVector> &target, size_t count, CastParameters &parameters) {
        bool result = GenericTryCastColumnVector<SourceType, TargetType, TryCastValueWithType<Operator>>(source, target, count, parameters);
        return result;
    }
};

} // namespace infinity