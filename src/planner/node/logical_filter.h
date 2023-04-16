//
// Created by JinHai on 2022/8/9.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"
#include "expression/base_expression.h"

namespace infinity {

class LogicalFilter : public LogicalNode {
public:
    explicit
    LogicalFilter(u64 node_id,
                  SharedPtr<BaseExpression> expression)
        : LogicalNode(node_id, LogicalNodeType::kFilter), expression_(std::move(expression)) {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return left_node_->GetColumnBindings();
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return left_node_->GetOutputNames();
    }

    [[nodiscard]] inline SharedPtr<Vector<DataType>>
    GetOutputTypes() const final {
        return left_node_->GetOutputTypes();
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalFilter";
    }

    [[nodiscard]] inline SharedPtr<BaseExpression>&
    expression() {
        return expression_;
    }

    [[nodiscard]] inline const SharedPtr<BaseExpression>&
    expression() const {
        return expression_;
    }
private:
    SharedPtr<BaseExpression> expression_{};
};

}

