//
// Created by jinhai on 23-2-5.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import base_expression;
import parser;

export module logical_aggregate;

namespace infinity {

export class LogicalAggregate : public LogicalNode {
public:
    explicit LogicalAggregate(u64 node_id,
                              Vector<SharedPtr<BaseExpression>> groups,
                              u64 groupby_index,
                              Vector<SharedPtr<BaseExpression>> aggregates,
                              u64 aggregate_index)
        : LogicalNode(node_id, LogicalNodeType::kAggregate), groups_(Move(groups)), groupby_index_(groupby_index),
          aggregates_(Move(aggregates)), aggregate_index_(aggregate_index) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalAggregate"; }

    Vector<SharedPtr<BaseExpression>> groups_{};
    u64 groupby_index_{};

    Vector<SharedPtr<BaseExpression>> aggregates_{};
    u64 aggregate_index_{};
};

} // namespace infinity
