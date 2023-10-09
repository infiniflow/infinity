//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalSort : public LogicalNode {
public:
    LogicalSort(u64 node_id, Vector<SharedPtr<BaseExpression>> expressions, Vector<OrderType> order_by_types)
        : LogicalNode(node_id, LogicalNodeType::kSort), expressions_(std::move(expressions)), order_by_types_(std::move(order_by_types)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final { return left_node_->GetColumnBindings(); }

    [[nodiscard]] inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_node_->GetOutputNames(); }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_node_->GetOutputTypes(); }

    String ToString(i64 &space) final;

    inline String name() final { return "LogicalSort"; }

    Vector<SharedPtr<BaseExpression>> expressions_{};
    Vector<OrderType> order_by_types_{};
};

} // namespace infinity