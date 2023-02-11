//
// Created by jinhai on 23-2-7.
//

#include "expression_evaluator.h"

#include <utility>

namespace infinity {

void
ExpressionEvaluator::Execute(const SharedPtr<BaseExpression>& expression,
                             SharedPtr<ExpressionState>& state,
                             HashMap<u64, SharedPtr<DataBlock>> input_block_map,
                             SizeT row_count,
                             SharedPtr<ColumnVector>& output_column_vector) {
    // Validate the input data block map
    ExecutorAssert(!input_block_map.empty(), "No input data block");

    blocks_map_ = std::move(input_block_map);
    ExecutorAssert(state != nullptr, "Expression state need to be initialized before.")
    ExecutorAssert(expression != nullptr, "No expression.")

    if(row_count == 0) {
        row_count = 1;
    }

    // output_column_vector needs to be initialized before.
    Execute(expression, state, output_column_vector, row_count);
}

void
ExpressionEvaluator::Execute(const SharedPtr<BaseExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column,
                             SizeT count) {

    switch(expr->type()) {
        case ExpressionType::kAggregate:
            return Execute(std::static_pointer_cast<AggregateExpression>(expr), state, output_column, count);
        case ExpressionType::kCast:
            return Execute(std::static_pointer_cast<CastExpression>(expr), state, output_column, count);
        case ExpressionType::kCase:
            return Execute(std::static_pointer_cast<CaseExpression>(expr), state, output_column, count);
        case ExpressionType::kConjunction:
            return Execute(std::static_pointer_cast<ConjunctionExpression>(expr), state, output_column, count);
        case ExpressionType::kColumn:
            return Execute(std::static_pointer_cast<ColumnExpression>(expr), state, output_column, count);
        case ExpressionType::kFunction:
            return Execute(std::static_pointer_cast<FunctionExpression>(expr), state, output_column, count);
        case ExpressionType::kBetween:
            return Execute(std::static_pointer_cast<BetweenExpression>(expr), state, output_column, count);
        case ExpressionType::kValue:
            return Execute(std::static_pointer_cast<ValueExpression>(expr), state, output_column, count);
        default:
            ExecutorError("Unknown expression type: " + expr->ToString());
    }
}

void
ExpressionEvaluator::Execute(const SharedPtr<AggregateExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {
    ExecutorAssert(!in_aggregate_, "Recursive execute aggregate function!")
    in_aggregate_ = true;
    SharedPtr<ExpressionState>& child_state = state->Children()[0];
    SharedPtr<BaseExpression>& child_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    SharedPtr<ColumnVector>& child_output = child_state->OutputColumnVector();
    Execute(child_expr, child_state, child_output, count);

    ExecutorAssert(expr->aggregate_function_.argument_type_ == child_output->data_type(),
                   "Argument type isn't matched with the child expression output");
    ExecutorAssert(expr->aggregate_function_.return_type_ == output_column_vector->data_type(),
                   "Return type isn't matched");

    // 1. Initialize the aggregate state.
    expr->aggregate_function_.init_func_(expr->aggregate_function_.GetState());

    // 2. Loop to fill the aggregate state
    expr->aggregate_function_.update_func_(expr->aggregate_function_.GetState(), child_output);

    // 3. Get the aggregate result and append to output column vector.

    ptr_t result_ptr = expr->aggregate_function_.finalize_func_(expr->aggregate_function_.GetState());
    output_column_vector->AppendByPtr(result_ptr);

    in_aggregate_ = false;
}

void
ExpressionEvaluator::Execute(const SharedPtr<CastExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {
    SharedPtr<ExpressionState>& child_state = state->Children()[0];
    SharedPtr<BaseExpression>& child_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    SharedPtr<ColumnVector>& child_output = child_state->OutputColumnVector();
    Execute(child_expr, child_state, child_output, count);

    ExecutorError("Cast function isn't implemented yet.");
}

void
ExpressionEvaluator::Execute(const SharedPtr<CaseExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {
    ExecutorError("Case execution isn't implemented yet.");
}

void
ExpressionEvaluator::Execute(const SharedPtr<ConjunctionExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {
    // Process left child expression
    SharedPtr<ExpressionState>& left_state = state->Children()[0];
    SharedPtr<BaseExpression>& left_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    SharedPtr<ColumnVector>& left_output = left_state->OutputColumnVector();
    Execute(left_expr, left_state, left_output, count);

    // Process right child expression
    SharedPtr<ExpressionState>& right_state = state->Children()[1];
    SharedPtr<BaseExpression>& right_expr = expr->arguments()[1];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    SharedPtr<ColumnVector>& right_output = right_state->OutputColumnVector();
    Execute(right_expr, right_state, right_output, count);

    ExecutorError("Conjunction function isn't implemented yet.");
}

void
ExpressionEvaluator::Execute(const SharedPtr<ColumnExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {

    i64 column_index = expr->column_index();
    u64 table_index = expr->table_index();
    ExecutorAssert(blocks_map_.contains(table_index), fmt::format("No table index: {}", table_index))
    const SharedPtr<DataBlock>& block_ptr = blocks_map_[table_index];
    ExecutorAssert(column_index < block_ptr->column_count(), "Invalid column index");
    output_column_vector = block_ptr->column_vectors[column_index];
}

void
ExpressionEvaluator::Execute(const SharedPtr<FunctionExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {

    SizeT argument_count = expr->arguments().size();
    Vector<SharedPtr<ColumnVector>> output_columns;
    output_columns.reserve(argument_count);
    for(SizeT i = 0; i < argument_count; ++ i) {
        SharedPtr<ExpressionState>& argument_state = state->Children()[i];
        SharedPtr<BaseExpression>& argument_expr = expr->arguments()[i];
        SharedPtr<ColumnVector>& argument_output = argument_state->OutputColumnVector();
        Execute(argument_expr, argument_state, argument_output, count);
        output_columns.emplace_back(argument_output);
    }
    DataBlock func_input_data_block;
    func_input_data_block.Init(output_columns);
    expr->func_.function_(func_input_data_block, output_column_vector);
}

void
ExpressionEvaluator::Execute(const SharedPtr<BetweenExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {

    // Lower expression execution
    SharedPtr<ExpressionState>& lower_state = state->Children()[0];
    SharedPtr<BaseExpression>& lower_expr = expr->arguments()[0];
    SharedPtr<ColumnVector>& lower_output = lower_state->OutputColumnVector();
    Execute(lower_expr, lower_state, lower_output, count);

    // Input expression execution
    SharedPtr<ExpressionState>& input_state = state->Children()[1];
    SharedPtr<BaseExpression>& input_expr = expr->arguments()[1];
    SharedPtr<ColumnVector>& input_output = input_state->OutputColumnVector();
    Execute(input_expr, input_state, input_output, count);

    // Upper expression execution
    SharedPtr<ExpressionState>& upper_state = state->Children()[1];
    SharedPtr<BaseExpression>& upper_expr = expr->arguments()[1];
    SharedPtr<ColumnVector>& upper_output = upper_state->OutputColumnVector();
    Execute(input_expr, input_state, input_output, count);

    ExecutorError("Between expression execution isn't implemented yet.");

}

void
ExpressionEvaluator::Execute(const SharedPtr<ValueExpression>& expr,
                             SharedPtr<ExpressionState>& state,
                             SharedPtr<ColumnVector>& output_column_vector,
                             SizeT count) {

}

}