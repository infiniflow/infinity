//
// Created by jinhai on 23-2-16.
//

#pragma once

#include "expression/aggregate_expression.h"
#include "expression/between_expression.h"
#include "expression/case_expression.h"
#include "expression/cast_expression.h"
#include "expression/column_expression.h"
#include "expression/conjunction_expression.h"
#include "expression/function_expression.h"
#include "expression/in_expression.h"
#include "expression/knn_expression.h"
#include "expression/subquery_expression.h"
#include "expression/value_expression.h"
#include "logical_node.h"

namespace infinity {

class LogicalNodeVisitor {
public:
    virtual void VisitNode(LogicalNode &op) = 0;

    void VisitNodeChildren(LogicalNode &op);

    void VisitNodeExpression(LogicalNode &op);

    void VisitExpression(SharedPtr<BaseExpression> &expression);

    void VisitExpressionChildren(SharedPtr<BaseExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<AggregateExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<BetweenExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<CaseExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<CastExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ConjunctionExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<FunctionExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ValueExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<InExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<SubqueryExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<KnnExpression> &expression);
};

} // namespace infinity
