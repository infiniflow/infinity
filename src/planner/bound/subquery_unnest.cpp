//
// Created by JinHai on 2022/9/26.
//

#include "subquery_unnest.h"

namespace infinity {

void
SubqueryUnnest::UnnestSubqueries(std::shared_ptr<BaseExpression> &expr_ptr, std::shared_ptr<LogicalNode> &root) {
    // 1. Go through all the expression to find subquery
    // 2. Call Unnest Subquery to resolve subquery
}

std::shared_ptr<BaseExpression>
SubqueryUnnest::UnnestSubquery(std::shared_ptr<BaseExpression> &expr_ptr, std::shared_ptr<LogicalNode> &root) {
    // 1. Check the subquery type: uncorrelated subquery or correlated subquery.
    // 2. Call different function to resolve uncorrelated subquery and correlated subquery.
    // 3. If the subquery also has another subquery nested, we need to resolve it recursively.

    return nullptr;
}

}

