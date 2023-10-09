//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalLimit : public LogicalNode {
public:
    explicit LogicalLimit(u64 node_id, SharedPtr<BaseExpression> limit_expression, SharedPtr<BaseExpression> offset_expression)
        : LogicalNode(node_id, LogicalNodeType::kLimit), limit_expression_(std::move(limit_expression)),
          offset_expression_(std::move(offset_expression)) {}

    [[nodiscard]] inline Vector<ColumnBinding> GetColumnBindings() const final { return left_node_->GetColumnBindings(); }

    [[nodiscard]] inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_node_->GetOutputNames(); }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_node_->GetOutputTypes(); }

    String ToString(i64 &space) final;

    inline String name() final { return "LogicalLimit"; }

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> offset_expression_{};
};

} // namespace infinity
