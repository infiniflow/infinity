//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/types/data_type.h"
#include "expression/base_expression.h"
#include "function.h"
#include "common/column_vector/column_vector.h"
#include <vector>

namespace infinity {

using AggregateInitializeFuncType = std::function<void(ptr_t)>;

class AggregateOperation {
public:
    template<typename AggregateState>
    static inline void
    StateInitialize(AggregateState* state) {
        state->initialize();
    }

    template<typename AggregateState, typename InputType>
    static inline void
    StateUpdate(AggregateState* state,
                const SharedPtr<ColumnVector>& input_column_vector) {
        // Loop execute state update according to the input column vector
        state->update();
    }

    template<typename AggregateState, typename ResultType>
    static inline void
    StateFinalize(AggregateState* state,
                  const SharedPtr<ColumnVector>& input_column_vector,
                  ResultType* result) {
        // Loop execute state update according to the input column vector
        *result = state->finalize();
    }
};

class AggregateFunction: public Function {
public:
    explicit
    AggregateFunction(String name,
                      DataType argument_type,
                      DataType return_type);

    void
    CastArgumentTypes(BaseExpression& input_argument);

    [[nodiscard]] const DataType&
    return_type() const {
        return return_type_;
    }

    [[nodiscard]] std::string
    ToString() const override;

private:
    DataType parameter_type_;
    DataType return_type_;

public:
    template <typename AggregateState, typename InputType, typename ResultType, typename AggregateOp>
    static inline AggregateFunction
    UnaryAggregate(const DataType& input_type, const DataType& return_type) {
        return AggregateFunction(
                {input_type},
                return_type,
                AggregateOperation::StateInitialize<AggregateState>,
                AggregateOperation::StateUpdate<AggregateState, InputType>,
                AggregateOperation::StateFinalize<AggregateState, ResultType>);
    }
};
}

