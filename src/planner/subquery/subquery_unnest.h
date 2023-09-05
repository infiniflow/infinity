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
                     QueryContext* query_context,
                     const SharedPtr<BindContext>& bind_context);

    static SharedPtr<BaseExpression>
    UnnestSubquery(SharedPtr<BaseExpression>& expr_ptr,
                   SharedPtr<LogicalNode>& root,
                   QueryContext* query_context,
                   const SharedPtr<BindContext>& bind_context);

    static SharedPtr<BaseExpression>
    UnnestUncorrelated(SubqueryExpression* expr_ptr,
                       SharedPtr<LogicalNode>& root,
                       SharedPtr<LogicalNode>& subquery_plan,
                       QueryContext* query_context,
                       const SharedPtr<BindContext>& bind_context);

    static SharedPtr<BaseExpression>
    UnnestCorrelated(SubqueryExpression* expr_ptr,
                     SharedPtr<LogicalNode>& root,
                     SharedPtr<LogicalNode>& subquery_plan,
                     QueryContext* query_context,
                     const SharedPtr<BindContext>& bind_context);

private:
    static void
    GenerateJoinConditions(QueryContext* query_context,
                           Vector<SharedPtr<BaseExpression>>& conditions,
                           const Vector<SharedPtr<ColumnExpression>>& correlated_columns,
                           const Vector<ColumnBinding>& subplan_column_bindings,
                           SizeT correlated_base_index);
};

}


