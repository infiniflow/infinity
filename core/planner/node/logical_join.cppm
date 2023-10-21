//
// Created by JinHai on 2022/9/25.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;
import base_expression;

export module logical_join;

namespace infinity {

export class LogicalJoin : public LogicalNode {
public:
    explicit LogicalJoin(u64 node_id,
                         JoinType join_type,
                         String alias,
                         Vector<SharedPtr<BaseExpression>> conditions,
                         const SharedPtr<LogicalNode> &left,
                         const SharedPtr<LogicalNode> &right);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalJoin"; }

    String alias_{};

    u64 mark_index_{}; // Only for mark join

public:
    JoinType join_type_{JoinType::kInner};
    Vector<SharedPtr<BaseExpression>> conditions_{};
};

} // namespace infinity
