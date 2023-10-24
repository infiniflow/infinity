//
// Created by jinhai on 23-4-9.
//

module;

import stl;
import logical_node_visitor;
import column_expression;
import base_expression;
import logical_node;
import subquery_expression;

export module corrlated_expr_detector;

namespace infinity {

export class CorrelatedExpressionsDetector final : public LogicalNodeVisitor {
public:
    explicit CorrelatedExpressionsDetector(const Vector<SharedPtr<ColumnExpression>> &correlated_column_exprs)
        : correlated_column_exprs_(correlated_column_exprs) {}

    void VisitNode(LogicalNode &op) final;

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) override;

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<SubqueryExpression> &expression) override;

    [[nodiscard]] inline bool IsCorrelated() const { return is_correlated_; }

private:
    const Vector<SharedPtr<ColumnExpression>> &correlated_column_exprs_;
    bool is_correlated_{false};
};

} // namespace infinity
