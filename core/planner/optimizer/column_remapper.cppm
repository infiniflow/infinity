//
// Created by jinhai on 23-2-17.
//

module;

import stl;
import logical_node_visitor;
import logical_node;
import base_expression;
import column_binding;
import query_context;
import column_expression;
import optimizer_rule;

export module column_remapper;

namespace infinity {

class BindingRemapper : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression);

    Vector<ColumnBinding> bindings_;
};

export class ColumnRemapper : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *query_context_ptr, const SharedPtr<LogicalNode> &logical_plan) final {
        return remapper_.VisitNode(*logical_plan);
    }

    [[nodiscard]] inline String name() const final { return "Column Remapper"; }

private:
    BindingRemapper remapper_{};
};

} // namespace infinity
