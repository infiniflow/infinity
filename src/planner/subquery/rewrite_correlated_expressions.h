//
// Created by jinhai on 23-4-10.
//

#pragma once

#include "planner/logical_node_visitor.h"

namespace infinity {

class RewriteCorrelatedExpressions final : public LogicalNodeVisitor {
public:
    explicit
    RewriteCorrelatedExpressions(const SharedPtr<BindContext> &bind_context_ptr,
                                 SizeT parent_table_index,
                                 SizeT left_offset,
                                 SizeT right_offset)
                                 :bind_context_ptr_(bind_context_ptr),
                                 parent_table_index_(parent_table_index),
                                 left_offset_(left_offset),
                                 right_offset_(right_offset)
    {}

    void
    VisitNode(LogicalNode& op) final;

    SharedPtr<BaseExpression>
    VisitReplace(const SharedPtr<ColumnExpression>& expression) final;

    SharedPtr<BaseExpression>
    VisitReplace(const SharedPtr<SubqueryExpression>& expression) final;

private:
    const SharedPtr<BindContext> &bind_context_ptr_;
    SizeT parent_table_index_{};
    SizeT left_offset_{};
    SizeT right_offset_{};
};

}
