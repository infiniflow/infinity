//
// Created by jinhai on 23-4-10.
//

#pragma once

#include "planner/logical_node_visitor.h"

namespace infinity {

class RewriteCorrelatedExpressions final : public LogicalNodeVisitor {
public:
    explicit
    RewriteCorrelatedExpressions(const SharedPtr<BindContext>& bind_context_ptr,
                                 ColumnBinding base_binding)
            : bind_context_ptr_(bind_context_ptr),
              base_binding_(base_binding) {}

    void
    VisitNode(LogicalNode& op) final;

    SharedPtr<BaseExpression>
    VisitReplace(const SharedPtr<ColumnExpression>& expression) final;

    SharedPtr<BaseExpression>
    VisitReplace(const SharedPtr<SubqueryExpression>& expression) final;

private:
    const SharedPtr<BindContext>& bind_context_ptr_;
    ColumnBinding base_binding_{};
};

}
