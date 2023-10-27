//
// Created by jinhai on 23-2-7.
//

// #include "expression_evaluator.h"
// #include "expression_state.h"
// #include "expression/aggregate_expression.h"
// #include "expression/base_expression.h"
// #include "expression/case_expression.h"
// #include "expression/cast_expression.h"
// #include "expression/column_expression.h"
// #include "expression/function_expression.h"
// #include "expression/in_expression.h"
// #include "expression/reference_expression.h"
// #include "expression/value_expression.h"

module;

#include <memory>

import stl;
import third_party;
import infinity_assert;
import infinity_exception;

import expression_type;

import aggregate_expression;
import base_expression;
import case_expression;
import cast_expression;
import column_expression;
import function_expression;
import in_expression;
import reference_expression;
import value_expression;

import expression_state;
import bound_cast_func;

module expression_evaluator;

namespace infinity {

void ExpressionEvaluator::Init(const DataBlock *input_data_block) { input_data_block_ = input_data_block; }

void ExpressionEvaluator::Execute(const SharedPtr<BaseExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column) {

    switch (expr->type()) {
        case ExpressionType::kAggregate:
            return Execute(std::static_pointer_cast<AggregateExpression>(expr), state, output_column);
        case ExpressionType::kCast:
            return Execute(std::static_pointer_cast<CastExpression>(expr), state, output_column);
        case ExpressionType::kCase:
            return Execute(std::static_pointer_cast<CaseExpression>(expr), state, output_column);
        case ExpressionType::kColumn:
            return Execute(std::static_pointer_cast<ColumnExpression>(expr), state, output_column);
        case ExpressionType::kFunction:
            return Execute(std::static_pointer_cast<FunctionExpression>(expr), state, output_column);
        case ExpressionType::kValue:
            return Execute(std::static_pointer_cast<ValueExpression>(expr), state, output_column);
        case ExpressionType::kReference:
            return Execute(std::static_pointer_cast<ReferenceExpression>(expr), state, output_column);
        case ExpressionType::kIn:
            return Execute(std::static_pointer_cast<InExpression>(expr), state, output_column);
        default: {
            Error<ExecutorException>("Unknown expression type: " + expr->Name(), __FILE_NAME__, __LINE__);
        }
    }
}

void ExpressionEvaluator::Execute(const SharedPtr<AggregateExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {
    Assert<ExecutorException>(!in_aggregate_, "Recursive execute aggregate function!", __FILE_NAME__, __LINE__);
    in_aggregate_ = true;
    SharedPtr<ExpressionState> &child_state = state->Children()[0];
    SharedPtr<BaseExpression> &child_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    SharedPtr<ColumnVector> &child_output = child_state->OutputColumnVector();
    Execute(child_expr, child_state, child_output);

    Assert<ExecutorException>(expr->aggregate_function_.argument_type_ == *child_output->data_type(),
                              "Argument type isn't matched with the child expression output",
                              __FILE_NAME__,
                              __LINE__);
    Assert<ExecutorException>(expr->aggregate_function_.return_type_ == *output_column_vector->data_type(),
                              "Return type isn't matched",
                              __FILE_NAME__,
                              __LINE__);

    // 1. Initialize the aggregate state.
    expr->aggregate_function_.init_func_(expr->aggregate_function_.GetState());

    // 2. Loop to fill the aggregate state
    expr->aggregate_function_.update_func_(expr->aggregate_function_.GetState(), child_output);

    // 3. Get the aggregate result and append to output column vector.

    ptr_t result_ptr = expr->aggregate_function_.finalize_func_(expr->aggregate_function_.GetState());
    output_column_vector->AppendByPtr(result_ptr);

    in_aggregate_ = false;
}

void ExpressionEvaluator::Execute(const SharedPtr<CastExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {
    SharedPtr<ExpressionState> &child_state = state->Children()[0];
    SharedPtr<BaseExpression> &child_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    SharedPtr<ColumnVector> &child_output = child_state->OutputColumnVector();
    Execute(child_expr, child_state, child_output);

    CastParameters cast_parameters;

    expr->func_.function(child_output, output_column_vector, child_output->Size(), cast_parameters);
}

void ExpressionEvaluator::Execute(const SharedPtr<CaseExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {
    Error<ExecutorException>("Case execution", __FILE_NAME__, __LINE__);
}

void ExpressionEvaluator::Execute(const SharedPtr<ColumnExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {
    Error<ExecutorException>("Column expression", __FILE_NAME__, __LINE__);
}

void ExpressionEvaluator::Execute(const SharedPtr<FunctionExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {

    SizeT argument_count = expr->arguments().size();
    Vector<SharedPtr<ColumnVector>> arguments;
    arguments.reserve(argument_count);

    for (SizeT i = 0; i < argument_count; ++i) {
        SharedPtr<ExpressionState> &argument_state = state->Children()[i];
        SharedPtr<BaseExpression> &argument_expr = expr->arguments()[i];
        SharedPtr<ColumnVector> &argument_output = argument_state->OutputColumnVector();
        Execute(argument_expr, argument_state, argument_output);
        arguments.emplace_back(argument_output);
    }

    DataBlock func_input_data_block;
    func_input_data_block.Init(arguments);

    expr->func_.function_(func_input_data_block, output_column_vector);
}

void ExpressionEvaluator::Execute(const SharedPtr<ValueExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {
    output_column_vector->SetValue(0, expr->GetValue());
    output_column_vector->Finalize(1);
}

void ExpressionEvaluator::Execute(const SharedPtr<ReferenceExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {
    SizeT column_index = expr->column_index();

    if (input_data_block_ == nullptr) {
        Error<ExecutorException>("Input data block is NULL", __FILE_NAME__, __LINE__);
    }
    if (column_index >= input_data_block_->column_count()) {
        Error<ExecutorException>("Invalid column index", __FILE_NAME__, __LINE__);
    }

    output_column_vector = input_data_block_->column_vectors[column_index];
}

void ExpressionEvaluator::Execute(const SharedPtr<InExpression> &expr,
                                  SharedPtr<ExpressionState> &state,
                                  SharedPtr<ColumnVector> &output_column_vector) {
    Error<ExecutorException>("IN execution isn't implemented yet.", __FILE_NAME__, __LINE__);
}

} // namespace infinity
