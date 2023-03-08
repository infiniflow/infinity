//
// Created by JinHai on 2022/10/26.
//

#include "common/utility/infinity_assert.h"
#include "expression_state.h"

namespace infinity {

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<BaseExpression> &expression, SizeT block_count) {

    switch(expression->type()) {
        case ExpressionType::kAggregate:
            return CreateState(std::static_pointer_cast<AggregateExpression>(expression), block_count);
        case ExpressionType::kCast:
            return CreateState(std::static_pointer_cast<CastExpression>(expression), block_count);
        case ExpressionType::kCase:
            return CreateState(std::static_pointer_cast<CaseExpression>(expression), block_count);
        case ExpressionType::kConjunction:
            return CreateState(std::static_pointer_cast<ConjunctionExpression>(expression), block_count);
        case ExpressionType::kFunction:
            return CreateState(std::static_pointer_cast<FunctionExpression>(expression), block_count);
        case ExpressionType::kBetween:
            return CreateState(std::static_pointer_cast<BetweenExpression>(expression), block_count);
        case ExpressionType::kValue:
            return CreateState(std::static_pointer_cast<ValueExpression>(expression), block_count);
        case ExpressionType::kReference:
            return CreateState(std::static_pointer_cast<ReferenceExpression>(expression), block_count);
        default:
            ExecutorError("Unknown expression type: " + expression->ToString());
    }

}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<AggregateExpression>& agg_expr, SizeT block_count) {
    ExecutorAssert(agg_expr->arguments().size() == 1, "Aggregate function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(agg_expr->arguments()[0], block_count);

    // Aggregate function will only have one output value.
    SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(agg_expr->Type());
    column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
    result->column_vectors_.emplace_back(column);
//    result->output_data_block_.Init({agg_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<BetweenExpression>& between_expr, SizeT block_count) {
    ExecutorAssert(between_expr->arguments().size() == 3, "Between expression arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();

    // input expression
    result->AddChild(between_expr->arguments()[0], block_count);

    // lower expression
    result->AddChild(between_expr->arguments()[1], block_count);

    // upper expression
    result->AddChild(between_expr->arguments()[2], block_count);

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(between_expr->Type());
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
//    result->output_data_block_.Init({between_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CaseExpression>& case_expr, SizeT block_count) {

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();

    Vector<CaseCheck>& case_checks = case_expr->CaseExpr();
    for(auto& case_check: case_checks) {
        result->AddChild(case_check.when_expr_, block_count);
        result->AddChild(case_check.then_expr_, block_count);
    }
    result->AddChild(case_expr->ElseExpr(), block_count);

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(case_expr->Type());
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
//    result->output_data_block_.Init({case_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CastExpression>& cast_expr, SizeT block_count) {
    ExecutorAssert(cast_expr->arguments().size() == 1, "Cast function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(cast_expr->arguments()[0], block_count);

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(cast_expr->Type());
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
//    result->output_data_block_.Init({cast_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ReferenceExpression>& column_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(column_expr->Type());

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(column_expr->Type());
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
//    result->output_data_block_.Init({column});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ConjunctionExpression>& conjunction_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = SharedPtr<ExpressionState>();
    ExecutorAssert(conjunction_expr->arguments().size() == 2, "Conjunction function arguments error.");

    result->AddChild(conjunction_expr->arguments()[0], block_count);
    result->AddChild(conjunction_expr->arguments()[1], block_count);

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(conjunction_expr->Type());
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<FunctionExpression>& function_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    for(auto& arg : function_expr->arguments()) {
        result->AddChild(arg, block_count);
    }

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(function_expr->Type());
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }

//    result->output_data_block_.Init({function_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ValueExpression>& value_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(value_expr->Type());
        column->Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        value_expr->AppendToChunk(column);
        result->column_vectors_.emplace_back(column);
    }

    return result;
}

void
ExpressionState::AddChild(const SharedPtr<BaseExpression> &expression, SizeT block_count) {
    children_.emplace_back(CreateState(expression, block_count));
}

}

