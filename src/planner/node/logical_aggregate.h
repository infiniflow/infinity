//
// Created by jinhai on 23-2-5.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalAggregate: public LogicalNode {
public:
    explicit
    LogicalAggregate(u64 node_id,
                     Vector<SharedPtr<BaseExpression>> groups,
                     u64 groupby_index,
                     Vector<SharedPtr<BaseExpression>> aggregates,
                     u64 aggregate_index)
            : LogicalNode(node_id, LogicalNodeType::kAggregate),
              groups_(std::move(groups)),
              groupby_index_(groupby_index),
              aggregates_(std::move(aggregates)),
              aggregate_index_(aggregate_index)
              {}

    [[nodiscard]] Vector<ColumnBinding>
    GetColumnBindings() const final;

    String
    ToString(i64 &space) final;

    inline String
    name() final {
        return "LogicalAggregate";
    }

    Vector<SharedPtr<BaseExpression>> groups_{};
    u64 groupby_index_{};

    Vector<SharedPtr<BaseExpression>> aggregates_{};
    u64 aggregate_index_{};
};

}
