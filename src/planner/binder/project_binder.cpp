//
// Created by JinHai on 2022/8/12.
//

#include "common/utility/infinity_assert.h"
#include "function/function_set.h"
#include "project_binder.h"
#include "parser/statement.h"
#include "expression/column_expression.h"

namespace infinity {

SharedPtr<BaseExpression>
ProjectBinder::BuildExpression(const hsql::Expr &expr,
                               const SharedPtr<BindContext> &bind_context_ptr,
                               i64 depth,
                               bool root) {
    String expr_name = expr.getName() == nullptr ? Statement::ExprAsColumnName(&expr) : expr.getName();

    // If the expr is coming from group by lists.
    if (bind_context_ptr->group_index_by_name_.contains(expr_name)) {
        i64 groupby_index = bind_context_ptr->group_index_by_name_[expr_name];
        const SharedPtr<BaseExpression>& group_expr = bind_context_ptr->group_exprs_[groupby_index];

        SharedPtr<ColumnExpression> result = ColumnExpression::Make(group_expr->Type(),
                                                                    bind_context_ptr->group_by_table_name_,
                                                                    std::to_string(groupby_index),
                                                                    groupby_index,
                                                                    depth);


        result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kGroupBy);
        return result;
    }

    // If the expr is coming from aggregate function list
    if(bind_context_ptr->aggregate_index_by_name_.contains(expr_name)) {
        i64 aggregate_index = bind_context_ptr->aggregate_index_by_name_[expr_name];
        const SharedPtr<BaseExpression>& aggregate_expr = bind_context_ptr->group_exprs_[aggregate_index];

        SharedPtr<ColumnExpression> result = ColumnExpression::Make(aggregate_expr->Type(),
                                                                    bind_context_ptr->aggregate_table_name_,
                                                                    std::to_string(aggregate_index),
                                                                    aggregate_index,
                                                                    depth);

        result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kAggregate);
        return result;
    }

    return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
}

SharedPtr<BaseExpression>
ProjectBinder::BuildFuncExpr(const hsql::Expr &expr,
                             const SharedPtr<BindContext>& bind_context_ptr,
                             i64 depth,
                             bool root) {

    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
    if(function_set_ptr->type_ == FunctionType::kAggregate) {
        if(this->binding_agg_func_) {
            PlannerError("Aggregate function is called in another aggregate function.");
        } else {
            this->binding_agg_func_ = true;
        }
    }
    auto func_expr_ptr = ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);

    if(function_set_ptr->type_ == FunctionType::kAggregate) {
        String expr_name = expr.getName();
        i64 aggregate_index = bind_context_ptr->aggregate_exprs_.size();
        bind_context_ptr->aggregate_exprs_.emplace_back(func_expr_ptr);
        bind_context_ptr->aggregate_index_by_name_[expr_name] = aggregate_index;

        this->binding_agg_func_ = false;
    }

    return func_expr_ptr;
}

SharedPtr<BaseExpression>
ProjectBinder::BuildColExpr(const hsql::Expr &expr,
                            const SharedPtr<BindContext>& bind_context_ptr,
                            i64 depth,
                            bool root) {
    auto bound_column_expr = ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
    if(bound_column_expr != nullptr && bound_column_name_.empty()) {
        bound_column_name_ = bound_column_expr->ToString();
    }
    return bound_column_expr;
}

}
