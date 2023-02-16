//
// Created by JinHai on 2022/10/26.
//

#include "common/utility/infinity_assert.h"
#include "expression_state.h"

namespace infinity {

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<BaseExpression> &expression) {

    switch(expression->type()) {
        case ExpressionType::kAggregate:
            return CreateState(std::static_pointer_cast<AggregateExpression>(expression));
        case ExpressionType::kCast:
            return CreateState(std::static_pointer_cast<CastExpression>(expression));
        case ExpressionType::kCase:
            return CreateState(std::static_pointer_cast<CaseExpression>(expression));
        case ExpressionType::kConjunction:
            return CreateState(std::static_pointer_cast<ConjunctionExpression>(expression));
        case ExpressionType::kColumn:
            return CreateState(std::static_pointer_cast<ColumnExpression>(expression));
        case ExpressionType::kFunction:
            return CreateState(std::static_pointer_cast<FunctionExpression>(expression));
        case ExpressionType::kBetween:
            return CreateState(std::static_pointer_cast<BetweenExpression>(expression));
        case ExpressionType::kValue:
            return CreateState(std::static_pointer_cast<ValueExpression>(expression));
        default:
            ExecutorError("Unknown expression type: " + expression->ToString());
    }

}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<AggregateExpression>& agg_expr) {
    ExecutorAssert(agg_expr->arguments().size() == 1, "Aggregate function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(agg_expr->arguments()[0]);
    result->output_data_block_.Init({agg_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<BetweenExpression>& between_expr) {
    ExecutorAssert(between_expr->arguments().size() == 3, "Between expression arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();

    // input expression
    result->AddChild(between_expr->arguments()[0]);

    // lower expression
    result->AddChild(between_expr->arguments()[1]);

    // upper expression
    result->AddChild(between_expr->arguments()[2]);

    result->output_data_block_.Init({between_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CaseExpression>& case_expr) {

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();

    Vector<CaseCheck>& case_checks = case_expr->CaseExpr();
    for(auto& case_check: case_checks) {
        result->AddChild(case_check.when_expr_);
        result->AddChild(case_check.then_expr_);
    }
    result->AddChild(case_expr->ElseExpr());

    result->output_data_block_.Init({case_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CastExpression>& cast_expr) {
    ExecutorAssert(cast_expr->arguments().size() == 1, "Cast function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(cast_expr->arguments()[0]);

    result->output_data_block_.Init({cast_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ColumnExpression>& column_expr) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(column_expr->Type());
    result->output_data_block_.Init({column});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ConjunctionExpression>& conjunction_expr) {
    SharedPtr<ExpressionState> result = SharedPtr<ExpressionState>();
    ExecutorAssert(conjunction_expr->arguments().size() == 2, "Conjunction function arguments error.");

    result->AddChild(conjunction_expr->arguments()[0]);
    result->AddChild(conjunction_expr->arguments()[1]);

    result->output_data_block_.Init({conjunction_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<FunctionExpression>& function_expr) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    for(auto& arg : function_expr->arguments()) {
        result->AddChild(arg);
    }

    result->output_data_block_.Init({function_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ValueExpression>& value_expr) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(value_expr->Type());
    column->Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    value_expr->AppendToChunk(column);
    result->output_data_block_.Init({column});
    return result;
}

void
ExpressionState::AddChild(const SharedPtr<BaseExpression> &expression) {
    children_.emplace_back(CreateState(expression));
}

}

