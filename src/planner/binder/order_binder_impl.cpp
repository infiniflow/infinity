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

module infinity_core:order_binder.impl;

import :order_binder;
import :stl;
import :base_expression;
import :bind_context;
import :column_expression;
import :status;
import :infinity_exception;
import :logger;

import third_party;

import parsed_expr;
import constant_expr;
import knn_expr;
import function_expr;

namespace infinity {

void OrderBinder::PushExtraExprToSelectList(ParsedExpr *expr, const SharedPtr<BindContext> &bind_context_ptr) {
    if (expr->type_ == ParsedExprType::kConstant) {
        ConstantExpr *order_by_index = (ConstantExpr *)expr;
        if (order_by_index->literal_type_ != LiteralType::kInteger) {
            Status status = Status::SyntaxError("Order by non-integer constant value.");
            RecoverableError(status);
        }
        // Order by 1, means order by 1st select list item.
        return;
    }

    String expr_name = expr->GetName();

    if (bind_context_ptr->binding_names_by_column_.contains(expr_name)) {
        expr_name = fmt::format("{}.{}", bind_context_ptr->binding_names_by_column_[expr_name][0], expr_name);
    }

    if (bind_context_ptr->select_alias2index_.contains(expr_name)) {
        return;
    }

    if (bind_context_ptr->select_expr_name2index_.contains(expr_name)) {
        return;
    }

    bind_context_ptr->select_expr_name2index_[expr_name] = bind_context_ptr->select_expression_.size();
    bind_context_ptr->select_expression_.emplace_back(expr);
}

SharedPtr<BaseExpression> OrderBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    String expr_name = expr.GetName();
    // If the expr isn't from aggregate function and coming from group by lists.
    if (bind_context_ptr->group_index_by_name_.contains(expr_name)) {
        i64 groupby_index = bind_context_ptr->group_index_by_name_[expr_name];
        const SharedPtr<BaseExpression> &group_expr = bind_context_ptr->group_exprs_[groupby_index];

        SharedPtr<ColumnExpression> result = ColumnExpression::Make(group_expr->Type(),
                                                                    bind_context_ptr->group_by_table_name_,
                                                                    bind_context_ptr->group_by_table_index_,
                                                                    expr_name,
                                                                    groupby_index,
                                                                    depth);

        result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kGroupBy);
        return result;
    }
    // If the expr is coming from aggregate function list
    if (bind_context_ptr->aggregate_index_by_name_.contains(expr_name)) {
        i64 aggregate_index = bind_context_ptr->aggregate_index_by_name_[expr_name];
        const SharedPtr<BaseExpression> &aggregate_expr = bind_context_ptr->aggregate_exprs_[aggregate_index];

        SharedPtr<ColumnExpression> result = ColumnExpression::Make(aggregate_expr->Type(),
                                                                    bind_context_ptr->aggregate_table_name_,
                                                                    bind_context_ptr->aggregate_table_index_,
                                                                    expr_name,
                                                                    aggregate_index,
                                                                    depth);

        result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kAggregate);
        return result;
    }
    if (!bind_context_ptr->aggregate_index_by_name_.empty() || !bind_context_ptr->group_index_by_name_.empty()) {
        String error_message = fmt::format("Column: {} must appear in the GROUP BY clause or be used in an aggregate function", expr_name);
        RecoverableError(Status::SyntaxError(error_message));
    }

    if (expr.type_ == ParsedExprType::kFunction or expr.type_ == ParsedExprType::kColumn) {
        return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    }
    if (expr.type_ == ParsedExprType::kKnn) {
        return ExpressionBinder::BuildKnnExpr((KnnExpr &)expr, bind_context_ptr, depth, root);
    }

    i64 column_id = -1;
    String binding_table_name = bind_context_ptr->project_table_name_;
    SizeT binding_table_index = bind_context_ptr->project_table_index_;

    // TODO: fix "order by 1" statement
    // If the expr is from projection, then create a column reference from projection.
    if (expr.type_ == ParsedExprType::kConstant) {
        ConstantExpr &const_expr = (ConstantExpr &)expr;
        if (const_expr.literal_type_ == LiteralType::kInteger) {
            column_id = const_expr.integer_value_;
            if (column_id <= 0 or column_id > (i64)bind_context_ptr->project_exprs_.size()) {
                Status status = Status::SyntaxError("Order by are going to use nonexistent column from select list.");
                RecoverableError(status);
            }
            --column_id;
            // TODO: If we do not have a projection before sort, expression will need to be evaluated twice
            //  now return shared_ptr of the chosen project_expr
            return bind_context_ptr->project_exprs_[column_id];
        } else {
            Status status = Status::SyntaxError("Order by non-integer constant value.");
            RecoverableError(status);
        }
    } else {
        String expr_name = expr.GetName();
        String error_message = fmt::format("Need to add support for {} in order_binder.", expr_name);
        UnrecoverableError(error_message);

        if (bind_context_ptr->binding_names_by_column_.contains(expr_name)) {
            auto table_name = bind_context_ptr->binding_names_by_column_[expr_name][0];
            expr_name = fmt::format("{}.{}", table_name, expr_name);
            binding_table_name = table_name;
            binding_table_index = bind_context_ptr->table_name2table_index_[table_name];
        }

        if (bind_context_ptr->select_alias2index_.contains(expr_name)) {
            column_id = bind_context_ptr->select_alias2index_[expr_name];
        }

        // TODO: bug: create table tt2 (x int, y int); select y from tt2 order by x;
        if (bind_context_ptr->select_expr_name2index_.contains(expr_name)) {
            column_id = bind_context_ptr->select_expr_name2index_[expr_name];
        }

        if (column_id == -1) {
            String error_message = fmt::format("{} isn't found in project list.", expr_name);
            UnrecoverableError(error_message);
        }
    }

    const SharedPtr<BaseExpression> &project_expr = bind_context_ptr->project_exprs_[column_id];

    SharedPtr<ColumnExpression> result =
        ColumnExpression::Make(project_expr->Type(), binding_table_name, binding_table_index, std::to_string(column_id), column_id, depth);
    result->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kProjection);
    return result;
}

} // namespace infinity
