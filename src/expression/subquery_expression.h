//
// Created by JinHai on 2022/9/19.
//

#pragma once

#include "base_expression.h"
#include "planner/bound_select_statement.h"

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
    explicit
    SubqueryExpression(SharedPtr<BoundSelectStatement> select_node, SubqueryType subquery_type);

    String
    ToString() const override;

    DataType
    Type() const override;

    SharedPtr<BoundSelectStatement> bound_select_statement_ptr_{};
    SubqueryType subquery_type_;

    // Used in IN subquery, EXISTS won't use it.
    SharedPtr<BaseExpression> left_{nullptr};
private:

};

}
