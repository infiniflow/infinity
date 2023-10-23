//
// Created by jinhai on 23-2-16.
//

module;

import stl;
import base_expression;
import logical_node;
import aggregate_expression;
import between_expression;
import case_expression;
import cast_expression;
import column_expression;
import function_expression;
import value_expression;
import in_expression;
import subquery_expression;
import knn_expression;
import conjunction_expression;

export module logical_node_visitor;

namespace infinity {

//class LogicalNode;
//class BaseExpression;
//class AggregateExpression;
//class BetweenExpression;
//class CaseExpression;
//class CastExpression;
//class ColumnExpression;
//class ConjunctionExpression;
//class FunctionExpression;
//class ValueExpression;
//class InExpression;
//class SubqueryExpression;
//class KnnExpression;

export class LogicalNodeVisitor {
public:
    virtual void VisitNode(LogicalNode &op) = 0;

    void VisitNodeChildren(LogicalNode &op);

    void VisitNodeExpression(LogicalNode &op);

    void VisitExpression(SharedPtr<BaseExpression> &expression);

    void VisitExpressionChildren(SharedPtr<BaseExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const AggregateExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const BetweenExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const CaseExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const CastExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const ColumnExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const ConjunctionExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const FunctionExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const ValueExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const InExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SubqueryExpression* expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const KnnExpression* expression);
};

} // namespace infinity
