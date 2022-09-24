//
// Created by JinHai on 2022/9/19.
//

#pragma once

#include "base_expression.h"
#include "planner/bound/bound_select_node.h"

namespace infinity {

enum class SubqueryType {
    kInvalid,
    kScalar,
    kExists,
    kNotExists,
    kIn,
    kNotIn,
    kAny,
};

class SubqueryExpression : public BaseExpression {
public:
    explicit SubqueryExpression(std::shared_ptr<BoundSelectNode> select_node, SubqueryType subquery_type);

    std::string ToString() const override;
    LogicalType DataType() override;

    std::shared_ptr<BoundSelectNode> select_node_ptr_;
    SubqueryType subquery_type_;
    ExpressionType operator_type_;
    std::shared_ptr<BaseExpression> left_;
private:

};

}
