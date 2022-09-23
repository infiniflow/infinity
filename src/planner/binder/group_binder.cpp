//
// Created by JinHai on 2022/8/11.
//

#include "common/utility/infinity_assert.h"
#include "function/function_set.h"
#include "group_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
GroupBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result;

    switch(expr.type) {
        case hsql::kExprFunctionRef: {
            std::shared_ptr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
            if(function_set_ptr->type_ != FunctionType::kScalar) {
                PlannerError("Only scalar function is supported in group by list.");
            }
        }
        case hsql::kExprSelect: {
            PlannerError("Subquery isn't supported in group by list.");
        }
        default: {
            result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
        }
    }

    // if(root_expression) {  } why we need root expression flag?
    {
        std::string expr_name = expr.getName();
        if(bind_context_ptr->groups_by_expr_.contains(expr_name)) {
            PlannerError("Duplicated group by expression: " + expr_name);
        }

        // Add the group by expression into bind context
        bind_context_ptr->groups_by_expr_[expr_name] = result;
        bind_context_ptr->groups_.emplace_back(result);
    }

    return result;
}

}
