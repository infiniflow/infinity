//
// Created by JinHai on 2022/8/11.
//

#include "common/utility/infinity_assert.h"
#include "function/function_set.h"
#include "group_binder.h"
#include "parser/statement.h"
#include "parser/expression/parsed_raw_expression.h"

namespace infinity {

SharedPtr<BaseExpression>
GroupBinder::BuildExpression(const hsql::Expr &expr,
                             const SharedPtr<BindContext>& bind_context_ptr,
                             i64 depth,
                             bool root) {
    if(depth == 0 && root) {
        switch(expr.type) {
            case hsql::kExprLiteralInt: {
                // Group by the expression of the given index in select list;
                // For example: select a, sum(a) from t1 group by 1; # group by a
                return BindConstantExpression(expr, bind_context_ptr);
            }
            case hsql::kExprColumnRef: {
                // Group by the column expression.
                // For example: select a as x from t1 group by x;
                return BindColumnReference(expr, bind_context_ptr);
            }
        }
    }

    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr,
                                                                         bind_context_ptr,
                                                                         depth,
                                                                         root);

    if(root) {
        String expr_name = expr.getName() == nullptr ? Statement::ExprAsColumnName(&expr) : expr.getName();

        if(bind_context_ptr->group_index_by_name_.contains(expr_name)) {
            PlannerError("Duplicated group by expression: " + expr_name);
        }

        // Add the group by expression into bind context
        bind_context_ptr->group_exprs_.emplace_back(result);
        bind_context_ptr->group_index_by_name_[expr_name] = this->group_by_expr_index;
    }

    return result;
}

SharedPtr<BaseExpression>
GroupBinder::BindColumnReference(const hsql::Expr &expr,
                                 const SharedPtr<BindContext>& bind_context_ptr) {
    // Either the expr is a column or a alias
    const char* name_ptr = expr.getName();
    PlannerAssert(name_ptr != nullptr, "Empty column name")
    String column_name = name_ptr;

    // Check if the column is from select list alias
    if(bind_context_ptr->select_alias2index_.contains(column_name)) {
        // Save this item is from group by output
        i64 select_expr_index = bind_context_ptr->select_alias2index_[column_name];
        bind_context_ptr->select_index_to_group_by_index_[select_expr_index] = this->group_by_expr_index;
    }

    // Check if the column is from select list column
    if(bind_context_ptr->select_expr_name2index_.contains(column_name)) {
        // Save this item is from group by output
        i64 select_expr_index = bind_context_ptr->select_expr_name2index_[column_name];
        bind_context_ptr->select_index_to_group_by_index_[select_expr_index] = this->group_by_expr_index;
    }

    // Bind it anyway.
    SharedPtr<BaseExpression> result = this->Bind(expr, bind_context_ptr, 0, false);
    return result;
}

SharedPtr<BaseExpression>
GroupBinder::BindConstantExpression(const hsql::Expr &expr,
                                    const SharedPtr<BindContext>& bind_context_ptr) {
    PlannerAssert(expr.isType(hsql::ExprType::kExprLiteralInt), "Not an integer.")
    i64 select_idx = expr.ival;

    Vector<SharedPtr<ParsedExpression>>&  expr_array = bind_context_ptr->select_expression_;
    if(select_idx > expr_array.size() or select_idx < 1) {
        std::stringstream ss;
        ss << "GROUP BY clause out of range - should be from 1 to "
           << expr_array.size()
           << std::endl;
        PlannerError(ss.str())
    }

    select_idx -= 1;

    SharedPtr<BaseExpression> result = nullptr;
    if(expr_array[select_idx]->type_ == ExpressionType::kRaw) {
        SharedPtr<ParsedRawExpression> parsed_raw_expr
                = std::static_pointer_cast<ParsedRawExpression>(expr_array[select_idx]);
        result = ExpressionBinder::BuildColExpr(*parsed_raw_expr->raw_expr_,
                                                bind_context_ptr,
                                                0,
                                                false);
    } else {
        SharedPtr<ParsedColumnExpression> parsed_col_expr
                = std::static_pointer_cast<ParsedColumnExpression>(expr_array[select_idx]);
        result = ExpressionBinder::BuildColExpr(parsed_col_expr,
                                                bind_context_ptr,
                                                0,
                                                false);
    }
    return result;
}

SharedPtr<BaseExpression>
GroupBinder::BuildColExpr(const hsql::Expr &expr,
                          const SharedPtr<BindContext>& bind_context_ptr,
                          i64 depth,
                          bool root) {

    // Check if the column is using an alias from select list.
    auto result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr, depth, root);

    if(result == nullptr) {
        result = ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
    }

    return result;
}

SharedPtr<BaseExpression>
GroupBinder::BuildFuncExpr(const hsql::Expr &expr,
                           const SharedPtr<BindContext>& bind_context_ptr,
                           i64 depth,
                           bool root) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
    if(function_set_ptr->type_ != FunctionType::kScalar) {
        PlannerError("Only scalar function is supported in group by list.");
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);
}

void
GroupBinder::CheckFuncType(FunctionType func_type) const {
    PlannerAssert(func_type == FunctionType::kScalar, "Onlyl scalar function type are allowed in group by clause");
}

SharedPtr<SubqueryExpression>
GroupBinder::BuildSubquery(const hsql::SelectStatement& select,
                           const SharedPtr<BindContext>& bind_context_ptr,
                           SubqueryType subquery_type,
                           i64 depth,
                           bool root) {
    PlannerError("Subquery isn't supported in group by list.");
}

}
