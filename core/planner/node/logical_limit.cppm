//
// Created by JinHai on 2022/9/26.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_limit;

namespace infinity {

class BaseExpression;

export class LogicalLimit : public LogicalNode {
public:
    explicit LogicalLimit(u64 node_id, SharedPtr<BaseExpression> limit_expression, SharedPtr<BaseExpression> offset_expression)
        : LogicalNode(node_id, LogicalNodeType::kLimit), limit_expression_(Move(limit_expression)),
          offset_expression_(Move(offset_expression)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalLimit"; }

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> offset_expression_{};
};

} // namespace infinity
