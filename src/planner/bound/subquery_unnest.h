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
    UnnestSubqueries(SharedPtr<BaseExpression>& expr_ptr,
                     SharedPtr<LogicalNode>& root,
                     const SharedPtr<QueryContext>& query_context_ptr,
                     const SharedPtr<BindContext>& bind_context);

    static SharedPtr<BaseExpression>
    UnnestSubquery(SharedPtr<BaseExpression>& expr_ptr,
                   SharedPtr<LogicalNode>& root,
                   const SharedPtr<QueryContext>& query_context_ptr,
                   const SharedPtr<BindContext>& bind_context);

    static SharedPtr<BaseExpression>
    UnnestUncorrelated(SubqueryExpression* expr_ptr,
                       SharedPtr<LogicalNode>& root,
                       SharedPtr<LogicalNode>& subquery_plan,
                       const SharedPtr<QueryContext>& query_context_ptr,
                       const SharedPtr<BindContext>& bind_context);
};

}


