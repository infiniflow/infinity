//
// Created by jinhai on 23-4-10.
//

#include "rewrite_correlated_expressions.h"
#include "expression/column_expression.h"
#include "expression/subquery_expression.h"
#include "planner/bind_context.h"

namespace infinity {

void RewriteCorrelatedExpressions::VisitNode(LogicalNode &op) { VisitNodeExpression(op); }

SharedPtr<BaseExpression> RewriteCorrelatedExpressions::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    if (expression->depth() == 0) {
        return expression;
    }

    if (expression->depth() > 1) {
        PlannerError("Correlated depth > 1 is not suppported now.")
    }

    auto entry = bind_context_ptr_->correlated_column_map_.find(expression->binding());
    if (entry == bind_context_ptr_->correlated_column_map_.end()) {
        // This column expression wasn't stored in correlated column map before
        PlannerError("Correlated expression isn't found.")
    }

    expression->SetBinding(base_binding_.table_idx, base_binding_.column_idx + entry->second);
    expression->SetDepth(0);

    return expression;
}

SharedPtr<BaseExpression> RewriteCorrelatedExpressions::VisitReplace(const SharedPtr<SubqueryExpression> &expression) {
    if (expression->correlated_columns.empty()) {
        // Not correlated
        return nullptr;
    }

    PlannerError("Not support rewrite nested correlated subquery in the subquery plan") return nullptr;
}

} // namespace infinity
