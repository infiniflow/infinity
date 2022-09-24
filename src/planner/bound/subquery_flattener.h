//
// Created by JinHai on 2022/9/24.
//

#pragma once

#include "expression/subquery_expression.h"
#include "bound_select_node.h"

namespace infinity {

class SubqueryFlattener {
public:
    explicit
    SubqueryFlattener(std::shared_ptr<BoundSelectNode>& bound_select_node, std::shared_ptr<BindContext> &bind_context_ptr);

    std::shared_ptr<BoundSelectNode>
    GetResult();
private:
    void
    FlattenWhereClause();

    void
    FlattenHavingList();

    void
    FlattenProjectList();

    std::shared_ptr<BaseExpression>
    TryToFlatten(std::shared_ptr<BaseExpression>& expr);

    std::shared_ptr<BaseExpression>
    FlattenSubquery(std::shared_ptr<SubqueryExpression>& subquery);

    std::shared_ptr<BoundSelectNode>& bound_select_node_;
    std::shared_ptr<BindContext> &bind_context_ptr_;
};

}