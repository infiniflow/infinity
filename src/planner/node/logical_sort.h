//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"
#include "parser/statement/select_statement.h"

namespace infinity {

class BaseExpression;

class LogicalSort : public LogicalNode {
public:
    LogicalSort(u64 node_id, Vector<SharedPtr<BaseExpression>> expressions, Vector<OrderType> order_by_types)
        : LogicalNode(node_id, LogicalNodeType::kSort), expressions_(std::move(expressions)), order_by_types_(std::move(order_by_types)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalSort"; }

    Vector<SharedPtr<BaseExpression>> expressions_{};
    Vector<OrderType> order_by_types_{};
};

} // namespace infinity