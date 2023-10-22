//
// Created by jinhai on 23-4-10.
//

module;

import stl;
import logical_node_visitor;
import logical_node;
import column_binding;
import base_expression;
import column_expression;
import subquery_expression;

export module rewrite_correlated_expression;

namespace infinity {

class BindContext;

export class RewriteCorrelatedExpressions : public LogicalNodeVisitor {
public:
    explicit RewriteCorrelatedExpressions(const SharedPtr<BindContext> &bind_context_ptr, ColumnBinding base_binding)
        : bind_context_ptr_(bind_context_ptr), base_binding_(base_binding) {}

    void VisitNode(LogicalNode &op) final;

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<SubqueryExpression> &expression);

private:
    const SharedPtr<BindContext> &bind_context_ptr_;
    ColumnBinding base_binding_{};
};

} // namespace infinity
