// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import base_expression;
import subquery_expression;
import parser;
import bind_context;
import function;
import expression_binder;

import infinity_exception;
import logger;
import third_party;
import function_set;
import bind_alias_proxy;

module group_binder;

namespace infinity {

SharedPtr<BaseExpression> GroupBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result = nullptr;

    if (depth == 0 && root) {
        switch (expr.type_) {
            case ParsedExprType::kConstant: {
                ConstantExpr &const_expr = (ConstantExpr &)expr;
                if (const_expr.literal_type_ == LiteralType::kInteger) {
                    // Group by the expression of the given index in select list;
                    // For example: select a, sum(a) from t1 group by 1; # group by a
                    result = BindConstantExpression(const_expr, bind_context_ptr);
                }
                break;
            }
            case ParsedExprType::kColumn: {
                // Group by the column expression.
                // For example: select a as x from t1 group by x;
                result = BindColumnReference((ColumnExpr &)expr, bind_context_ptr);
                break;
            }
            default: {
                LOG_TRACE("Go to bind non-constant and column expression in group binder");
                break;
            }
        }
    }

    if (result.get() == nullptr) {
        // Group by expr isn't constant int or column expression same as select list.
        result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    }

    if (root) {
        String expr_name = expr.GetName();

        if (bind_context_ptr->group_index_by_name_.contains(expr_name)) {
            Error<PlannerException>(Format("Duplicated group by expression: {}", expr_name));
        }

        // Add the group by expression into bind context
        bind_context_ptr->group_exprs_.emplace_back(result);
        bind_context_ptr->group_index_by_name_[expr_name] = this->group_by_expr_index;
    }

    return result;
}

SharedPtr<BaseExpression> GroupBinder::BindColumnReference(const ColumnExpr &expr, BindContext *bind_context_ptr) {
    // TODO: refactor:
    // - Check if the column is from base table.
    // - Check if the column is from an alias of select list expression.
    // - Check if the column is from outer queries.

    // Either the expr is a column or a alias
    String column_name = expr.GetName();

    // Check if the column is from select list alias
    if (bind_context_ptr->select_alias2index_.contains(column_name)) {
        // Save this item is from group by output
        i64 select_expr_index = bind_context_ptr->select_alias2index_[column_name];
        bind_context_ptr->select_index_to_group_by_index_[select_expr_index] = this->group_by_expr_index;
    }

    // Check if the column is from select list column
    if (bind_context_ptr->select_expr_name2index_.contains(column_name)) {
        // Save this item is from group by output
        i64 select_expr_index = bind_context_ptr->select_expr_name2index_[column_name];
        bind_context_ptr->select_index_to_group_by_index_[select_expr_index] = this->group_by_expr_index;
    }

    // Bind it anyway.
    SharedPtr<BaseExpression> result = this->Bind(expr, bind_context_ptr, 0, false);
    return result;
}

SharedPtr<BaseExpression> GroupBinder::BindConstantExpression(const ConstantExpr &expr, BindContext *bind_context_ptr) {
    if (expr.literal_type_ != LiteralType::kInteger) {
        Error<PlannerException>("Not an integer.");
    }
    i64 select_idx = expr.integer_value_;

    Vector<ParsedExpr *> &expr_array = bind_context_ptr->select_expression_;
    if (select_idx > expr_array.size() or select_idx < 1) {
        Error<PlannerException>(Format("GROUP BY clause out of range - should be from 1 to {}", expr_array.size()));
    }

    select_idx -= 1;

    ColumnExpr &col_expr = (ColumnExpr &)(*expr_array[select_idx]);

    SharedPtr<BaseExpression> result = ExpressionBinder::BuildColExpr(col_expr, bind_context_ptr, 0, false);
    return result;
}

SharedPtr<BaseExpression> GroupBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {

    // Check if the column is using an alias from select list.
    auto result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr, depth, root);

    if (result.get() == nullptr) {
        result = ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
    }

    return result;
}

SharedPtr<BaseExpression> GroupBinder::BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), expr);
    if (function_set_ptr->type_ != FunctionType::kScalar) {
        Error<PlannerException>("Only scalar function is supported in group by list.");
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);
}

void GroupBinder::CheckFuncType(FunctionType func_type) const {
    if (func_type != FunctionType::kScalar) {
        Error<PlannerException>("Only scalar function is supported in group by list.");
    }
}

SharedPtr<SubqueryExpression>
GroupBinder::BuildSubquery(const SubqueryExpr &, BindContext *, SubqueryType , i64 , bool ) {
    Error<PlannerException>("Subquery isn't supported in group by list.");
    return nullptr;
}

SharedPtr<BaseExpression> GroupBinder::BuildKnnExpr(const KnnExpr &, BindContext *, i64 , bool ) {
    Error<PlannerException>("KNN expression isn't supported in group by list");
    return nullptr;
}

} // namespace infinity
