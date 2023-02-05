//
// Created by JinHai on 2022/9/24.
//

#pragma once

#include "expression/subquery_expression.h"

namespace infinity {

class SubqueryFlattener {
public:
    explicit
    SubqueryFlattener(SharedPtr<BoundSelectStatement>& bound_select_statement, SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<BoundSelectStatement>
    GetResult();
private:
    void
    FlattenWhereClause();

    void
    FlattenHavingList();

    void
    FlattenProjectList();

    SharedPtr<BaseExpression>
    TryToFlatten(SharedPtr<BaseExpression>& expr);

    SharedPtr<BaseExpression>
    FlattenSubquery(SharedPtr<SubqueryExpression>& subquery);

    SharedPtr<BoundSelectStatement>& bound_select_statement_;
    SharedPtr<BindContext> &bind_context_ptr_;
};

}