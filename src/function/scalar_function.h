//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function.h"
#include "storage/data_type.h"
#include "storage/transient_block.h"
#include "expression/base_expression.h"
#include "executor/operation_state.h"
#include "common/utility/infinity_assert.h"
#include "common/vector/unary_operation.h"
#include "common/vector/binary_operation.h"

#include <vector>

namespace infinity {

class ScalarFunction : public Function {
public:
    explicit ScalarFunction(std::string name,
                            std::vector<LogicalType> argument_types,
                            LogicalType return_type);

    void CastArgumentTypes(std::vector<BaseExpression>& input_arguments);

public:
    static void NoOpFunction(const TransientBlock& input, Chunk& output);

    template<typename InputType, typename OutputType, typename Operation>
    static void UnaryFunction(const TransientBlock& input, Chunk& output) {
        ExecutorAssert(input.ColumnCount() == 1, "Unary function: input column count isn't one.");
        UnaryOperation::Execute<InputType, OutputType, Operation>(input, output);
    }

    template<typename LeftType, typename RightType, typename OutputType, typename OperationType>
    static void BinaryFunction(const TransientBlock& input, Chunk& output) {
        ExecutorAssert(input.ColumnCount() == 2, "Binary function: input column count isn't two.");
        ExecutorAssert(input.chunks_[0].row_count() == input.chunks_[1].row_count(), "Two input chunk row count are mismatched.");
        BinaryOperation::Execute<LeftType, RightType, OutputType, OperationType>(input.chunks_[0], input.chunks_[1], output);
    }
private:
    std::vector<LogicalType> argument_types_;
    LogicalType return_type_;

};
}

