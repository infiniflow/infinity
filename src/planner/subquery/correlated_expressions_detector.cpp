//
// Created by jinhai on 23-4-9.
//

#include "correlated_expressions_detector.h"

namespace infinity {

void CorrelatedExpressionsDetector::VisitNode(LogicalNode &op) { VisitNodeExpression(op); }

SharedPtr<BaseExpression> CorrelatedExpressionsDetector::VisitReplace(const SharedPtr<ColumnExpression> &expression) {

    if (expression->depth() == 0) {
        return expression;
    }

    if (expression->depth() > 1) {
        PlannerError("Column expression with depth > 1 is detected")
    }

    is_correlated_ = true;
    return expression;
}

SharedPtr<BaseExpression> CorrelatedExpressionsDetector::VisitReplace(const SharedPtr<SubqueryExpression> &expression) {
    if (expression->correlated_columns.empty()) {
        // Uncorrelated subquery
        return nullptr;
    }

    PlannerError("Not support nested correlated subquery in the subquery plan")
}

} // namespace infinity
