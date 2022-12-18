//
// Created by JinHai on 2022/12/15.
//

#pragma once
#include "common/column_vector/operator/unary_operator.h"
#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "common/types/null_value.h"

namespace infinity {

struct ColumnVectorCastData {
    explicit
    ColumnVectorCastData(bool strict) : strict_(strict) {}

    LogicalType source_type_{LogicalType::kInvalid};
    LogicalType target_type_{LogicalType::kInvalid};
    bool strict_{false};
    bool all_converted_{true};
};


template<typename Operator>
struct ValueTryCast {
    template<typename SourceValueType, typename TargetValueType>
    inline static TargetValueType
    Execute(SourceValueType input, Bitmask* nulls_ptr, size_t idx, void* state_ptr) {
        TargetValueType result;
        if(Operator::template Run<SourceValueType, TargetValueType>(input, result)) {
            return result;
        }

        auto* data_ptr = (ColumnVectorCastData*)(state_ptr);
        // This convert is failed
        data_ptr->all_converted_ = false;
//        data_ptr->source_type_  =
//        data_ptr->target_type_  =
//        data_ptr->result_ref_
        return NullValue<TargetValueType>();
    }
};

struct ColumnVectorCast {

template <class SourceType, class TargetType, class Operator>
static bool
GenericTryCastColumnVector(const ColumnVector &source, ColumnVector &result, size_t count, CastParameters &parameters) {
    ColumnVectorCastData input(parameters.strict);
    UnaryOperation::Execute<SourceType, TargetType, Operator>(source, result, count, &input, true);
    return input.all_converted_;
}

template <class SourceType, class TargetType, class Operator>
inline static bool
TryCastColumnVector(const ColumnVector &source, ColumnVector &target, size_t count, CastParameters &parameters) {
    bool result = GenericTryCastColumnVector<SourceType, TargetType, ValueTryCast<Operator>>(source, target, count, parameters);
    return result;
}

};

}