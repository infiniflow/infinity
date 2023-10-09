//
// Created by JinHai on 2022/8/11.
//

#include "having_binder.h"
#include "common/utility/infinity_assert.h"
#include "expression/column_expression.h"
#include "function/function_set.h"

namespace infinity {

SharedPtr<BaseExpression> HavingBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {

    String expr_name = expr.GetName();

    // Trying to bind in group by
    if (!this->binding_agg_func_) {
        if (bind_context_ptr->group_index_by_name_.contains(expr_name)) {
            i64 groupby_index = bind_context_ptr->group_index_by_name_[expr_name];
            const SharedPtr<BaseExpression> &group_expr = bind_context_ptr->group_exprs_[groupby_index];

            SharedPtr<ColumnExpression> result = ColumnExpression::Make(group_expr->Type(),
                                                                        bind_context_ptr->group_by_table_name_,
                                                                        bind_context_ptr->group_by_table_index_,
                                                                        std::to_string(groupby_index),
                                                                        groupby_index,
                                                                        depth);

            result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kGroupBy);
            return result;
        }
    }

    if (bind_context_ptr->aggregate_index_by_name_.contains(expr_name)) {
        if (!this->binding_agg_func_) {
            // not in an aggregate function
            i64 aggregate_index = bind_context_ptr->aggregate_index_by_name_[expr_name];
            const SharedPtr<BaseExpression> &aggregate_expr = bind_context_ptr->aggregate_exprs_[aggregate_index];

            SharedPtr<ColumnExpression> result = ColumnExpression::Make(aggregate_expr->Type(),
                                                                        bind_context_ptr->aggregate_table_name_,
                                                                        bind_context_ptr->aggregate_table_index_,
                                                                        std::to_string(aggregate_index),
                                                                        aggregate_index,
                                                                        depth);

            result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kAggregate);
            return result;
        } else {
            // in an aggregate function, which means aggregate function nested, which is error.
            PlannerError("Aggregate function is called in another aggregate function.");
        }
    }

    return ExpressionBinder::BuildExpression(expr, bind_context_ptr, 0, false);
}

SharedPtr<BaseExpression> HavingBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {

    // SELECT sum(a) from t1 group by b having sum(a) > 0;
    if (this->binding_agg_func_) {
        // Try to bind the column expression in aggregate function.

        // Check if the column is using an alias from select list.
        auto result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr, depth, root);

        if (result == nullptr) {
            result = ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
        }

        return result;

    } else {
        PlannerError("Column " + String(expr.GetName()) + " must appear in the GROUP BY clause or be used in an aggregate function");
    }
}

SharedPtr<BaseExpression> HavingBinder::BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {

    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), expr);
    if (function_set_ptr->type_ == FunctionType::kAggregate) {
        if (this->binding_agg_func_) {
            PlannerError("Aggregate function is called in another aggregate function.");
        } else {
            this->binding_agg_func_ = true;
        }
    }
    auto func_expr_ptr = ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);

    if (function_set_ptr->type_ == FunctionType::kAggregate) {
        // SELECT sum(a) from t1 group by b having sum(a) > 0;
        // sum(a) is bound here

        String expr_name = expr.GetName();
        i64 aggregate_index = bind_context_ptr->aggregate_exprs_.size();
        bind_context_ptr->aggregate_exprs_.emplace_back(func_expr_ptr);
        bind_context_ptr->aggregate_index_by_name_[expr_name] = aggregate_index;

        this->binding_agg_func_ = false;
        SharedPtr<ColumnExpression> result = ColumnExpression::Make(func_expr_ptr->Type(),
                                                                    bind_context_ptr->aggregate_table_name_,
                                                                    bind_context_ptr->aggregate_table_index_,
                                                                    std::to_string(aggregate_index),
                                                                    aggregate_index,
                                                                    depth);

        return result;
    }

    return func_expr_ptr;
}

SharedPtr<BaseExpression> HavingBinder::BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    PlannerError("KNN expression isn't supported in having clause")
}

} // namespace infinity