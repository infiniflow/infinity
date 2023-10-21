//
// Created by JinHai on 2022/9/24.
//

module;

import stl;
import bind_context;
import query_context;
import logical_node;
import column_binding;

export module dependent_join_flattener;

namespace infinity {

export class DependentJoinFlattener {
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
