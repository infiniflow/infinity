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
import parser;
import bind_context;
import function;
import function_set;
import column_expression;
import third_party;

import infinity_exception;

module project_binder;

namespace {

using namespace infinity;

void ConvertAvgToSumDivideCount(FunctionExpr &func_expression, const Vector<String> &column_names) {
    func_expression.func_name_ = "/";
    func_expression.arguments_->clear();
    auto createFunctionWithColumnArg = [&column_names](const String &func_name) {
        auto function_expression = MakeUnique<FunctionExpr>();
        function_expression->func_name_ = func_name;
        function_expression->arguments_ = new Vector<ParsedExpr *>();
        auto column_expr = MakeUnique<ColumnExpr>();
        column_expr->names_.push_back(column_names[0]);
        function_expression->arguments_->push_back(column_expr.release());
        return function_expression.release();
    };
    func_expression.arguments_->push_back(createFunctionWithColumnArg("sum"));
    func_expression.arguments_->push_back(createFunctionWithColumnArg("count"));
}

} // namespace

namespace infinity {

SharedPtr<BaseExpression> ProjectBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    String expr_name = expr.GetName();

    // Covert avg function expr to (sum / count) function expr
    if (expr.type_ == ParsedExprType::kFunction) {
        auto &function_expression = (FunctionExpr &)expr;
        auto special_function = TryBuildSpecialFuncExpr(function_expression, bind_context_ptr, depth);
        if (special_function.has_value()) {
            return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
        }
        auto function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), function_expression);

        if (IsEqual(function_set_ptr->name(), String("AVG")) && function_expression.arguments_->size() == 1 &&
            (*function_expression.arguments_)[0]->type_ == ParsedExprType::kColumn) {
            auto column_expr = (ColumnExpr *)(*function_expression.arguments_)[0];
            Vector<String> column_names(Move(column_expr->names_));
            delete column_expr;
            ConvertAvgToSumDivideCount(function_expression, column_names);
            return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
        }
    }
    // If the expr isn't from aggregate function and coming from group by lists.
    if (!this->binding_agg_func_ && bind_context_ptr->group_index_by_name_.contains(expr_name)) {
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

    if (expr.HasAlias()) {
        expr_name = expr.ToString();
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
    }

    return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
}

SharedPtr<BaseExpression> ProjectBinder::BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    auto special_function = TryBuildSpecialFuncExpr(expr, bind_context_ptr, depth);
    if (special_function.has_value()) {
        return special_function.value();
    }

    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), expr);
    if (function_set_ptr->type_ == FunctionType::kAggregate) {
        if (this->binding_agg_func_) {
            Error<PlannerException>(Format("Aggregate function {} is called in another aggregate function.", function_set_ptr->name()));
        } else {
            this->binding_agg_func_ = true;
        }
    }
    auto func_expr_ptr = ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);

    if (function_set_ptr->type_ == FunctionType::kAggregate) {
        String expr_name = expr.GetName();
        i64 aggregate_index = bind_context_ptr->aggregate_exprs_.size();
        bind_context_ptr->aggregate_exprs_.emplace_back(func_expr_ptr);
        bind_context_ptr->aggregate_index_by_name_[expr_name] = aggregate_index;

        SharedPtr<ColumnExpression> column_expr_ptr = ColumnExpression::Make(func_expr_ptr->Type(),
                                                                             bind_context_ptr->aggregate_table_name_,
                                                                             bind_context_ptr->aggregate_table_index_,
                                                                             expr_name,
                                                                             aggregate_index,
                                                                             depth);

        column_expr_ptr->source_position_ = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kAggregate);
        this->binding_agg_func_ = false;

        return column_expr_ptr;
    }

    return func_expr_ptr;
}

SharedPtr<BaseExpression> ProjectBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    auto bound_column_expr = ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
    if (bound_column_expr.get() != nullptr && bound_column_name_.empty() && !binding_agg_func_) {
        // Not binding Aggregate function argument.
        bound_column_name_ = bound_column_expr->Name();
    }
    return bound_column_expr;
}

} // namespace infinity
