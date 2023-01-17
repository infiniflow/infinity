//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "expression/base_expression.h"
#include "expression/subquery_expression.h"
#include "planner/logical_node.h"

namespace infinity {

class SubqueryUnnest {
public:
    static void
    UnnestSubqueries(SharedPtr<BaseExpression>& expr_ptr, SharedPtr<LogicalNode>& root);

    static SharedPtr<BaseExpression>
    UnnestSubquery(SharedPtr<BaseExpression>& expr_ptr, SharedPtr<LogicalNode>& root);

    static SharedPtr<BaseExpression>
    UnnestUncorrelated(SharedPtr<SubqueryExpression>& expr_ptr,
                       SharedPtr<LogicalNode>& left,
                       SharedPtr<LogicalNode>& right);
};

}


