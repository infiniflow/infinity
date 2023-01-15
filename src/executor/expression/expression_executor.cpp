//
// Created by JinHai on 2022/10/26.
//

#include "expression_executor.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

void
ExpressionExecutor::Init(const Vector<SharedPtr<BaseExpression>> &exprs) {
    for(auto& expr_ptr: exprs) {
        expressions.emplace_back(expr_ptr);
        states.emplace_back(ExpressionState::CreateState(expr_ptr));
    }
}

void
ExpressionExecutor::Execute(SharedPtr<Table> &input_table, SharedPtr<Table> &output_table) {
    ExecutorAssert(!expressions.empty(), "No expression.");
    SizeT expression_count = expressions.size();
    ExecutorAssert(expression_count == output_table->ColumnCount(), "Expression execution output columns count is mismatched.");

//    table_map_.emplace(input_table->TableName(), input_table);

    // Output data block types
    Vector<DataType> output_types;
    output_types.reserve(expression_count);

    for(SizeT idx = 0; idx < expression_count; ++ idx) {
        output_types.emplace_back(expressions[idx]->Type());
    }

    SizeT input_data_block_count = input_table->DataBlockCount();
    for(SizeT idx = 0; idx < input_data_block_count; ++ idx) {
        SharedPtr<DataBlock> output_data_block = DataBlock::Make();
        output_data_block->Init(output_types);

        Execute(input_table->GetDataBlockById(idx), output_data_block);
        output_data_block->Finalize();
        output_table->Append(output_data_block);
    }
}

void
ExpressionExecutor::Execute(const SharedPtr<DataBlock>& input_data_block, SharedPtr<DataBlock>& output_data_block) {
    // Set input data block as the input data of the executor.
    this->input_data_ = input_data_block;
    SizeT row_count = input_data_block->row_count() == 0 ? 1 : input_data_block->row_count();

    SizeT expression_output_count = expressions.size();
    for(SizeT idx = 0; idx < expression_output_count; ++ idx) {
        Execute(expressions[idx], states[idx], output_data_block->column_vectors[idx], row_count);
    }
}

void
ExpressionExecutor::Execute(SharedPtr<BaseExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column,
                            size_t count) {

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
ExpressionExecutor::Execute(const SharedPtr<AggregateExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {
    SharedPtr<ExpressionState>& child_state = state->Children()[0];
    SharedPtr<BaseExpression>& child_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    SharedPtr<ColumnVector>& child_output = child_state->OutputColumnVector();
    Execute(child_expr, child_state, child_output, count);

    ExecutorError("Aggregate function isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const SharedPtr<CastExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {
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
ExpressionExecutor::Execute(const SharedPtr<CaseExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {
    ExecutorError("Case execution isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const SharedPtr<ConjunctionExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {
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
ExpressionExecutor::Execute(const SharedPtr<ColumnExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {

    i64 column_index = expr->column_index();
    ExecutorAssert(column_index < this->input_data_->column_count(), "Invalid column index");
    output_column_vector = this->input_data_->column_vectors[column_index];
}

void
ExpressionExecutor::Execute(const SharedPtr<FunctionExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {

    size_t argument_count = expr->arguments().size();
    Vector<SharedPtr<ColumnVector>> output_columns;
    output_columns.reserve(argument_count);
    for(size_t i = 0; i < argument_count; ++ i) {
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
ExpressionExecutor::Execute(const SharedPtr<BetweenExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {

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
ExpressionExecutor::Execute(const SharedPtr<ValueExpression>& expr,
                            SharedPtr<ExpressionState>& state,
                            SharedPtr<ColumnVector>& output_column_vector,
                            size_t count) {

}

}
