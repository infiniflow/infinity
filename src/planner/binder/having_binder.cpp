//
// Created by JinHai on 2022/8/11.
//

#include "common/utility/infinity_assert.h"
#include "function/function_set.h"
#include "having_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
HavingBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {

    std::string expr_name = expr.getName();
    if(!this->binding_agg_func_) {
        if (bind_context_ptr->groups_by_expr_.contains(expr_name)) {
            return bind_context_ptr->groups_by_expr_[expr_name];
        }
    }

    if(bind_context_ptr->aggregates_by_expr_.contains(expr_name)) {
        if(!this->binding_agg_func_) {
            // not in an aggregate function
            return bind_context_ptr->aggregates_by_expr_[expr_name];
        } else {
            // in an aggregate function, which means aggregate function nested, which is error.
            PlannerError("Aggregate function is called in another aggregate function.");
        }
    }

    return ExpressionBinder::BuildExpression(expr, bind_context_ptr);
}

std::shared_ptr<BaseExpression>
HavingBinder::BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    if(this->binding_agg_func_) {
        return ExpressionBinder::BuildColExpr(expr, bind_context_ptr);
    } else {
        PlannerError("Column " + std::string(expr.getName()) + " must appear in the GROUP BY clause or be used in an aggregate function");
    }
}

std::shared_ptr<BaseExpression>
HavingBinder::BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {

    std::shared_ptr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
    if(function_set_ptr->type_ == FunctionType::kAggregate) {
        this->binding_agg_func_ = true;
    }
    auto result = ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr);

    if(function_set_ptr->type_ == FunctionType::kAggregate) {
        std::string expr_name = expr.getName();
        bind_context_ptr->aggregates_by_expr_.emplace(expr_name, result);

        this->binding_agg_func_ = false;
    }

    return result;
}

}