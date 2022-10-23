//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function.h"
#include "storage/data_type.h"
#include "storage/transblock.h"
#include "expression/base_expression.h"
#include "executor/operation_state.h"
#include "common/utility/infinity_assert.h"
#include "common/vector/unary_operation.h"
#include "common/vector/binary_operation.h"

#include <vector>

namespace infinity {

using ScalarFunctionType = std::function<void(const TransBlock&, Chunk &)>;

class ScalarFunction : public Function {
public:
    explicit ScalarFunction(std::string name,
                            std::vector<LogicalType> argument_types,
                            LogicalType return_type,
                            ScalarFunctionType function);

    void CastArgumentTypes(std::vector<BaseExpression>& input_arguments);
    [[nodiscard]] const LogicalType& return_type() const { return return_type_; }

    [[nodiscard]] std::string
    ToString() const override;
public:
    static void NoOpFunction(const TransBlock& input, Chunk& output);

    template<typename InputType, typename OutputType, typename Operation>
    static void UnaryFunction(const TransBlock& input, Chunk& output) {
        ExecutorAssert(input.ColumnCount() == 1, "Unary function: input column count isn't one.");
        UnaryOperation::Execute<InputType, OutputType, Operation>(input.chunks_[0], output);
    }

    template<typename LeftType, typename RightType, typename OutputType, typename OperationType>
    static void BinaryFunction(const TransBlock& input, Chunk& output) {
        ExecutorAssert(input.ColumnCount() == 2, "Binary function: input column count isn't two.");
        ExecutorAssert(input.chunks_[0].row_count() == input.chunks_[1].row_count(), "Two input chunk row count are mismatched.");
        BinaryOperation::Execute<LeftType, RightType, OutputType, OperationType>(input.chunks_[0], input.chunks_[1], output);
    }

    std::vector<LogicalType> parameter_types_;
    LogicalType return_type_;

    ScalarFunctionType function_;

};
}

