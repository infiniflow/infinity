//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function.h"
#include "expression/base_expression.h"
#include "executor/operation_state.h"
#include "common/utility/infinity_assert.h"
#include "common/column_vector/operator/unary_operator.h"
#include "common/column_vector/operator/binary_operator.h"
#include "storage/data_block.h"

#include <vector>

namespace infinity {

using ScalarFunctionType = std::function<void(const DataBlock&, ColumnVector&)>;

class ScalarFunction : public Function {
public:
    explicit ScalarFunction(String name,
                            Vector<DataType> argument_types,
                            DataType return_type,
                            ScalarFunctionType function);

    void
    CastArgumentTypes(Vector<BaseExpression>& input_arguments);

    [[nodiscard]] const DataType&
    return_type() const { return return_type_; }

    [[nodiscard]] String
    ToString() const override;
public:
    static void
    NoOpFunction(const DataBlock& input, ColumnVector& output);

    template<typename InputType, typename OutputType, typename Operation>
    static void
    UnaryFunction(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 1, "Unary function: input column count isn't one.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        UnaryOperator::Execute<InputType, OutputType, Operation>(
                input.column_vectors[0],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    template<typename LeftType, typename RightType, typename OutputType, typename OperationType>
    static void
    BinaryFunction(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 2, "Binary function: input column count isn't two.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        BinaryOperator::Execute<LeftType, RightType, OutputType, OperationType>(
                input.column_vectors[0],
                input.column_vectors[1],
                output,
                input.row_count(),
                true);
    }

    Vector<DataType> parameter_types_;
    DataType return_type_;

    ScalarFunctionType function_;

};
}

