//
// Created by JinHai on 2022/9/24.
//

#include "subquery_flattener.h"
#include "common/utility/infinity_assert.h"
#include "expression/case_expression.h"

namespace infinity {

SubqueryFlattener::SubqueryFlattener(std::shared_ptr<BoundSelectNode>& bound_select_node,
                                     std::shared_ptr<BindContext> &bind_context_ptr)
                                     : bound_select_node_(bound_select_node), bind_context_ptr_(bind_context_ptr)
                                     {}

std::shared_ptr<BoundSelectNode>
SubqueryFlattener::GetResult() {
    FlattenWhereClause();
    FlattenHavingList();
    FlattenProjectList();
    return bound_select_node_;
}

void
SubqueryFlattener::FlattenWhereClause() {
    for(auto& condition: bound_select_node_->where_conditions_) {
        TryToFlatten(condition);
    }
}

void
SubqueryFlattener::FlattenHavingList() {

}

void
SubqueryFlattener::FlattenProjectList() {

}

std::shared_ptr<BaseExpression>
SubqueryFlattener::TryToFlatten(std::shared_ptr<BaseExpression>& expression) {
    switch(expression->type()) {
        case ExpressionType::kArithmetic:
        case ExpressionType::kConjunction:
        case ExpressionType::kFunction:
        case ExpressionType::kCast: {
            for(auto& arg_expr: expression->arguments()) {
                TryToFlatten(arg_expr);
            }
            break;
        }
        case ExpressionType::kCase: {
            std::shared_ptr<CaseExpression> case_expr = std::static_pointer_cast<CaseExpression>(expression);
            PlannerError("Case expression is special.");
            break;
        }
        case ExpressionType::kSubQuery: {
            std::shared_ptr<SubqueryExpression> subquery_expr = std::static_pointer_cast<SubqueryExpression>(expression);
            return FlattenSubquery(subquery_expr);
        }
        case ExpressionType::kColumn: PlannerError("Column doesn't need to flatten subquery.");
        case ExpressionType::kCorrelatedColumn: PlannerError("Correlated Column doesn't need to flatten subquery.");
        case ExpressionType::kValue: PlannerError("Constant value doesn't need to flatten subquery.");
        case ExpressionType::kAggregate: PlannerError("Aggregate function shouldn't be in where clause");
        default: PlannerError("Invalid expression type in where clause.");
    }

    PlannerError("Shouldn't be here.");
}

std::shared_ptr<BaseExpression>
SubqueryFlattener::FlattenSubquery(std::shared_ptr<SubqueryExpression>& subquery) {

    // Pull up the correlated predicate
    PlannerError("Not implemented, yet.");
}

}
