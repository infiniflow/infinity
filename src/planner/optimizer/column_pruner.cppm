//
// Created by kould on 23-10-25.
//

module;

import stl;
import logical_node_visitor;
import reference_expression;
import base_expression;
import column_expression;
import function_expression;
import column_binding;
import bind_context;
import optimizer_rule;
import logical_node;
import query_context;

export module column_pruner;

namespace infinity {

class RemoveUnusedColumns : public LogicalNodeVisitor {
public:
    explicit RemoveUnusedColumns(bool is_root = false)
        : all_referenced_(is_root) {
    }

    void VisitNode(LogicalNode &op) final;

private:
    SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) final;

    template <class T>
    Vector<T> ClearUnusedExpressions(const Vector<T> &list, idx_t table_idx);

    bool all_referenced_;
    HashSet<ColumnBinding> column_references_;
};

export class ColumnPruner : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *query_context_ptr, const SharedPtr<LogicalNode> &logical_plan) final {
        return remove_visitor.VisitNode(*logical_plan);
    }

    [[nodiscard]] inline String name() const final { return "Column Pruner"; }

private:
    RemoveUnusedColumns remove_visitor{true};
};

}