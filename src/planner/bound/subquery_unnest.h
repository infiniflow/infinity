//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "expression/base_expression.h"
#include "planner/logical_node.h"

namespace infinity {

class SubqueryUnnest {
public:
    static void
    UnnestSubqueries(std::shared_ptr<BaseExpression>& expr_ptr, std::shared_ptr<LogicalNode>& root);

    static std::shared_ptr<BaseExpression>
    UnnestSubquery(std::shared_ptr<BaseExpression>& expr_ptr, std::shared_ptr<LogicalNode>& root);
};

}


