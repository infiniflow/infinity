//
// Created by jinhai on 23-2-17.
//

#pragma once

#include "planner/logical_node_visitor.h"
#include "planner/optimizer_rule.h"

namespace infinity {

class BindingRemapper : public LogicalNodeVisitor {
public:
    void VisitNode(LogicalNode &op) final;

private:
    SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) final;

    Vector<ColumnBinding> bindings_;
};

class ColumnRemapper : public OptimizerRule {
public:
    inline void ApplyToPlan(QueryContext *query_context_ptr, const SharedPtr<LogicalNode> &logical_plan) final {
        return remapper_.VisitNode(*logical_plan);
    }

    [[nodiscard]] inline String name() const final { return "Column Remapper"; }

private:
    BindingRemapper remapper_{};
};

} // namespace infinity
