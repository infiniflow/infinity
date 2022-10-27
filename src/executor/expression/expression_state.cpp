//
// Created by JinHai on 2022/10/26.
//

#include "common/utility/infinity_assert.h"
#include "expression_state.h"

namespace infinity {

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<BaseExpression> &expression) {

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

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<AggregateExpression>& agg_expr) {
    ExecutorAssert(agg_expr->arguments().size() == 1, "Aggregate function arguments error.");

    std::shared_ptr<ExpressionState> result = std::make_shared<ExpressionState>();
    result->AddChild(agg_expr->arguments()[0]);
    result->output_block_.Init({agg_expr->DataType()});
    return result;
}

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<BetweenExpression>& between_expr) {
    ExecutorAssert(between_expr->arguments().size() == 3, "Between expression arguments error.");

    std::shared_ptr<ExpressionState> result = std::make_shared<ExpressionState>();

    // input expression
    result->AddChild(between_expr->arguments()[0]);

    // lower expression
    result->AddChild(between_expr->arguments()[1]);

    // upper expression
    result->AddChild(between_expr->arguments()[2]);

    result->output_block_.Init({between_expr->DataType()});
    return result;
}

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<CaseExpression>& case_expr) {

    std::shared_ptr<ExpressionState> result = std::make_shared<ExpressionState>();

    std::vector<CaseCheck>& case_checks = case_expr->CaseExpr();
    for(auto& case_check: case_checks) {
        result->AddChild(case_check.when_expr_);
        result->AddChild(case_check.then_expr_);
    }
    result->AddChild(case_expr->ElseExpr());

    result->output_block_.Init({case_expr->DataType()});
    return result;
}

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<CastExpression>& cast_expr) {
    ExecutorAssert(cast_expr->arguments().size() == 1, "Cast function arguments error.");

    std::shared_ptr<ExpressionState> result = std::make_shared<ExpressionState>();
    result->AddChild(cast_expr->arguments()[0]);

    result->output_block_.Init({cast_expr->DataType()});
    return result;
}

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<ColumnExpression>& column_expr) {
    std::shared_ptr<ExpressionState> result = std::make_shared<ExpressionState>();

    // TODO: output_block
    return result;
}

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<ConjunctionExpression>& conjunction_expr) {
    std::shared_ptr<ExpressionState> result = std::shared_ptr<ExpressionState>();
    ExecutorAssert(conjunction_expr->arguments().size() == 2, "Conjunction function arguments error.");

    result->AddChild(conjunction_expr->arguments()[0]);
    result->AddChild(conjunction_expr->arguments()[1]);

    result->output_block_.Init({conjunction_expr->DataType()});
    return result;
}

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<FunctionExpression>& function_expr) {
    std::shared_ptr<ExpressionState> result = std::shared_ptr<ExpressionState>();
    for(auto& arg : function_expr->arguments()) {
        result->AddChild(arg);
    }

    result->output_block_.Init({function_expr->DataType()});
    return result;
}

std::shared_ptr<ExpressionState>
ExpressionState::CreateState(const std::shared_ptr<ValueExpression>& value_expr) {
    std::shared_ptr<ExpressionState> result = std::shared_ptr<ExpressionState>();

    // TODO: output_block, initialized here?
    return result;
}

void
ExpressionState::AddChild(const std::shared_ptr<BaseExpression> &expression) {
    children_.emplace_back(CreateState(expression));
}

}

