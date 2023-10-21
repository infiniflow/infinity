//
// Created by JinHai on 2022/9/26.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_sort;

namespace infinity {

class BaseExpression;

export class LogicalSort : public LogicalNode {
public:
    inline LogicalSort(u64 node_id, Vector<SharedPtr<BaseExpression>> expressions, Vector<OrderType> order_by_types)
        : LogicalNode(node_id, LogicalNodeType::kSort), expressions_(Move(expressions)), order_by_types_(Move(order_by_types)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalSort"; }

    Vector<SharedPtr<BaseExpression>> expressions_{};
    Vector<OrderType> order_by_types_{};
};

} // namespace infinity