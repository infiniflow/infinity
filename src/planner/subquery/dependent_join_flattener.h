//
// Created by JinHai on 2022/9/24.
//

#pragma once

#include "expression/subquery_expression.h"
#include "planner/column_binding.h"

namespace infinity {

class DependentJoinFlattener {
public:
    explicit DependentJoinFlattener(const SharedPtr<BindContext> &bind_context_ptr, QueryContext *query_context)
        : bind_context_ptr_(bind_context_ptr), query_context_(query_context) {}

    bool DetectCorrelatedExpressions(const SharedPtr<LogicalNode> &logical_node);

    SharedPtr<LogicalNode> PushDependentJoin(const SharedPtr<LogicalNode> &logical_node);

    inline SizeT CorrelatedColumnBaseIndex() const { return correlated_expression_offset_; }

private:
    SharedPtr<LogicalNode> PushDependentJoinInternal(const SharedPtr<LogicalNode> &logical_node);

    SharedPtr<LogicalNode> BuildNoCorrelatedInternal(const SharedPtr<LogicalNode> &logical_node);

private:
    const SharedPtr<BindContext> &bind_context_ptr_;
    QueryContext *query_context_{};
    HashMap<u64, bool> operator2correlated_expression_map_;
    ColumnBinding base_binding_{};
    SizeT correlated_expression_offset_{};
};

} // namespace infinity
