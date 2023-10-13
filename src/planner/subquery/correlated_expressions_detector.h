//
// Created by jinhai on 23-4-9.
//

#pragma once

#include "planner/logical_node_visitor.h"
#include "common/types/alias/containers.h"

namespace infinity {

class CorrelatedExpressionsDetector final : public LogicalNodeVisitor {
public:
    explicit CorrelatedExpressionsDetector(const Vector<SharedPtr<ColumnExpression>> &correlated_column_exprs)
        : correlated_column_exprs_(correlated_column_exprs) {}

    void VisitNode(LogicalNode &op) final;

    SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) final;

    SharedPtr<BaseExpression> VisitReplace(const SharedPtr<SubqueryExpression> &expression) final;

    [[nodiscard]] inline bool IsCorrelated() const { return is_correlated_; }

private:
    const Vector<SharedPtr<ColumnExpression>> &correlated_column_exprs_;
    bool is_correlated_{false};
};

} // namespace infinity
