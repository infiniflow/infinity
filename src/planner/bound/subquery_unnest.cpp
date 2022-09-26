//
// Created by JinHai on 2022/9/26.
//

#include "subquery_unnest.h"
#include "expression/expression_transformer.h"

#include "common/utility/infinity_assert.h"

namespace infinity {

void
SubqueryUnnest::UnnestSubqueries(std::shared_ptr<BaseExpression> &expr_ptr, std::shared_ptr<LogicalNode> &root) {
    // 1. Go through all the expression to find subquery
    VisitExpression(expr_ptr,
                    [&](std::shared_ptr<BaseExpression> &expr) {
                        UnnestSubqueries(expr, root);
                    });
    // 2. Call Unnest Subquery to resolve subquery
    if (expr_ptr->type() == ExpressionType::kSubQuery) {
        // Subquery, need to be unnested.
        UnnestSubquery(expr_ptr, root);
    }
}

std::shared_ptr<BaseExpression>
SubqueryUnnest::UnnestSubquery(std::shared_ptr<BaseExpression>& expr_ptr, std::shared_ptr<LogicalNode>& root) {
    // 1. Check the subquery type: uncorrelated subquery or correlated subquery.
    auto subquery_expr = std::static_pointer_cast<SubqueryExpression>(expr_ptr);

    auto right = subquery_expr->select_node_ptr_->BuildPlan();
    // TODO: if the correlated information of the subquery should be stored in bind context.
    // Check the correlated information
    auto result = UnnestUncorrelated(subquery_expr, root, right);
    // If it isn't a correlated subquery

    // 2. Call different function to resolve uncorrelated subquery and correlated subquery.
    // 3. If the subquery also has another subquery nested, we need to resolve it recursively.

    return result;
}

std::shared_ptr<BaseExpression>
SubqueryUnnest::UnnestUncorrelated(std::shared_ptr<SubqueryExpression>& expr_ptr,
                   std::shared_ptr<LogicalNode>& left,
                   std::shared_ptr<LogicalNode>& right) {
    switch(expr_ptr->subquery_type_) {

        case SubqueryType::kScalar:
            PlannerError("Plan SCALAR uncorrelated subquery");
            break;
        case SubqueryType::kExists:
            // Construct following plan tree:
            // CrossProduct
            // |-> left plan tree
            // |-> Project (count_star = 1)
            //     |-> Aggregate( count(*) as count_start)
            //         |-> Limit (1)
            //             |-> right plan tree
            PlannerError("Plan EXISTS uncorrelated subquery");
            break;
        case SubqueryType::kNotExists:
            PlannerError("Plan not EXISTS uncorrelated subquery");
            break;
        case SubqueryType::kIn:
            PlannerError("Plan IN uncorrelated subquery");
            break;
        case SubqueryType::kNotIn:
            PlannerError("Plan not IN uncorrelated subquery");
            break;
        case SubqueryType::kAny:
            PlannerError("Plan ANY uncorrelated subquery");
            break;
        default:
            PlannerError("Unknown subquery type.");
    }

    PlannerError("Not implement to unnest uncorrelated subquery.");
}

}

