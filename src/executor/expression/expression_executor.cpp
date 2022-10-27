//
// Created by JinHai on 2022/10/26.
//

#include "expression_executor.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

void
ExpressionExecutor::Init(const std::vector<std::shared_ptr<BaseExpression>> &exprs) {
    for(auto& expr_ptr: exprs) {
        expressions.emplace_back(expr_ptr);
        states.emplace_back(ExpressionState::CreateState(expr_ptr));
    }
}

void
ExpressionExecutor::Execute(std::shared_ptr<Table> &input, std::shared_ptr<Table> &output) {
    ExecutorAssert(!expressions.empty(), "No expression.");
    size_t output_column_count = expressions.size();
    ExecutorAssert(output_column_count == output->table_def()->column_count(), "Output column count is mismatched.");

    std::vector<Chunk>& output_columns = output->blocks()[0]->columns();
    size_t count = (input->row_count() == 0) ? 1 : input->row_count();

    table_map_.emplace(input->table_def()->name(), input);

    for(size_t i = 0; i < output_column_count; ++ i) {
        Execute(expressions[i], states[i], output_columns[i], count);
    }

}

void
ExpressionExecutor::Execute(std::shared_ptr<BaseExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
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
ExpressionExecutor::Execute(const std::shared_ptr<AggregateExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {
    std::shared_ptr<ExpressionState>& child_state = state->Children()[0];
    std::shared_ptr<BaseExpression>& child_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    Chunk& child_output = child_state->OutputChunk();
    Execute(child_expr, child_state, child_output, count);

    ExecutorError("Aggregate function isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const std::shared_ptr<CastExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {
    std::shared_ptr<ExpressionState>& child_state = state->Children()[0];
    std::shared_ptr<BaseExpression>& child_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    Chunk& child_output = child_state->OutputChunk();
    Execute(child_expr, child_state, child_output, count);

    ExecutorError("Cast function isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const std::shared_ptr<CaseExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {
    ExecutorError("Case execution isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const std::shared_ptr<ConjunctionExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {
    // Process left child expression
    std::shared_ptr<ExpressionState>& left_state = state->Children()[0];
    std::shared_ptr<BaseExpression>& left_expr = expr->arguments()[0];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    Chunk& left_output = left_state->OutputChunk();
    Execute(left_expr, left_state, left_output, count);

    // Process right child expression
    std::shared_ptr<ExpressionState>& right_state = state->Children()[1];
    std::shared_ptr<BaseExpression>& right_expr = expr->arguments()[1];
    // Create output chunk.
    // TODO: Now output chunk is pre-allocate memory in expression state
    // TODO: In the future, it can be implemented as on-demand allocation.
    Chunk& right_output = right_state->OutputChunk();
    Execute(right_expr, right_state, right_output, count);

    ExecutorError("Conjunction function isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const std::shared_ptr<ColumnExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {
    const std::string& table_name = expr->table_name();
    if(!table_map_.contains(table_name)) {
        ExecutorError("Table: " + table_name + ", doesn't exist.");
    }
    std::shared_ptr<Table>& table_ptr = table_map_.at(expr->table_name());
    const std::string& column_name = expr->column_name();
    int64_t column_index = expr->column_index();

    int64_t table_column_count = table_ptr->table_def()->column_count();
    if(column_index >= table_column_count) {
        ExecutorError("Table: " + table_name + ", column count: " + std::to_string(table_column_count)
                        + ", expect to access column index: " + std::to_string(column_index) );
    }

    const std::string& target_column_name = table_ptr->table_def()->columns()[column_index].name();
    if(column_name != target_column_name) {
        ExecutorError("Table: " + table_name + ", expect column name: " + column_name + ", at "
                        + std::to_string(column_index) + ", actually column name: " + target_column_name );
    }

    // FIXME: Now, the output column is copied from input table which should only be a reference operation but not copy.
    output_column = table_ptr->blocks()[0]->columns()[column_index];
}

void
ExpressionExecutor::Execute(const std::shared_ptr<FunctionExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {
    size_t argument_count = expr->arguments().size();
    for(size_t i = 0; i < argument_count; ++ i) {
        std::shared_ptr<ExpressionState>& argument_state = state->Children()[i];
        std::shared_ptr<BaseExpression>& argument_expr = expr->arguments()[i];
        Chunk& argument_output = argument_state->OutputChunk();
        Execute(argument_expr, argument_state, argument_output, count);
    }

    ExecutorError("Function expression execution isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const std::shared_ptr<BetweenExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {

    // Lower expression execution
    std::shared_ptr<ExpressionState>& lower_state = state->Children()[0];
    std::shared_ptr<BaseExpression>& lower_expr = expr->arguments()[0];
    Chunk& lower_output = lower_state->OutputChunk();
    Execute(lower_expr, lower_state, lower_output, count);

    // Input expression execution
    std::shared_ptr<ExpressionState>& input_state = state->Children()[1];
    std::shared_ptr<BaseExpression>& input_expr = expr->arguments()[1];
    Chunk& input_output = input_state->OutputChunk();
    Execute(input_expr, input_state, input_output, count);

    // Upper expression execution
    std::shared_ptr<ExpressionState>& upper_state = state->Children()[1];
    std::shared_ptr<BaseExpression>& upper_expr = expr->arguments()[1];
    Chunk& upper_output = upper_state->OutputChunk();
    Execute(input_expr, input_state, input_output, count);

    ExecutorError("Between expression execution isn't implemented yet.");

}

void
ExpressionExecutor::Execute(const std::shared_ptr<ValueExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {

}

}
