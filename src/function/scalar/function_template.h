//
// Created by jinhai on 22-12-27.
//

#pragma once

#include "common/column_vector/operator/unary_operator.h"

#include "common/types/data_type.h"
#include "common/types/null_value.h"

namespace infinity {

struct UnaryMathFunctionData {
    bool all_converted_{false};
};

template <typename Operator>
struct TryUnaryMathFunction {
    template <typename SourceValueType, typename TargetValueType>
    inline static TargetValueType Execute(SourceValueType input, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        return Operator::template Run<SourceValueType, TargetValueType>(input);
    }
};

} // namespace infinity
