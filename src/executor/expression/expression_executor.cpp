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
    // TODO: Use self-defined memory allocator to allocate chunk memory
    Chunk child_output(child_expr->DataType(), count);
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
    // TODO: Use self-defined memory allocator to allocate chunk memory
    Chunk child_output(child_expr->DataType(), count);
    Execute(child_expr, child_state, child_output, count);

    ExecutorError("Cast function isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const std::shared_ptr<CaseExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {

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
    // TODO: Use self-defined memory allocator to allocate chunk memory
    Chunk left_output(left_expr->DataType(), count);
    Execute(left_expr, left_state, left_output, count);

    // Process right child expression
    std::shared_ptr<ExpressionState>& right_state = state->Children()[1];
    std::shared_ptr<BaseExpression>& right_expr = expr->arguments()[1];
    // Create output chunk.
    // TODO: Use self-defined memory allocator to allocate chunk memory
    Chunk right_output(right_expr->DataType(), count);
    Execute(right_expr, right_state, right_output, count);

    ExecutorError("Conjunction function isn't implemented yet.");
}

void
ExpressionExecutor::Execute(const std::shared_ptr<ColumnExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {

}

void
ExpressionExecutor::Execute(const std::shared_ptr<FunctionExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {

}

void
ExpressionExecutor::Execute(const std::shared_ptr<BetweenExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {

}

void
ExpressionExecutor::Execute(const std::shared_ptr<ValueExpression>& expr,
                            std::shared_ptr<ExpressionState>& state,
                            Chunk& output_column,
                            size_t count) {

}

}
