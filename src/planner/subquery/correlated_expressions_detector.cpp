//
// Created by jinhai on 23-4-9.
//

module;

import stl;
import logical_node;
import base_expression;
import column_expression;
import subquery_expression;

import infinity_exception;

module corrlated_expr_detector;

namespace infinity {

void CorrelatedExpressionsDetector::VisitNode(LogicalNode &op) { VisitNodeExpression(op); }

SharedPtr<BaseExpression> CorrelatedExpressionsDetector::VisitReplace(const SharedPtr<ColumnExpression> &expression) {

    if (expression->depth() == 0) {
        return expression;
    }

    if (expression->depth() > 1) {
        Error<PlannerException>("Column expression with depth > 1 is detected");
    }

    is_correlated_ = true;
    return expression;
}

SharedPtr<BaseExpression> CorrelatedExpressionsDetector::VisitReplace(const SharedPtr<SubqueryExpression> &expression) {
    if (expression->correlated_columns.empty()) {
        // Uncorrelated subquery
        return nullptr;
    }

    Error<PlannerException>("Not support nested correlated subquery in the subquery plan");
}

} // namespace infinity
