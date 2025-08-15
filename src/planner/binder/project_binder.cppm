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

export module infinity_core:project_binder;

import :stl;
import :base_expression;
import :bind_context;
import :expression_binder;
import :bound_select_statement;

import parsed_expr;
import function_expr;
import column_expr;

namespace infinity {

export class ProjectBinder final : public ExpressionBinder {
public:
    explicit ProjectBinder(QueryContext *query_context) : ExpressionBinder(query_context) {}
    ProjectBinder(QueryContext *query_context, BoundSelectStatement *bound_select_statement)
        : ExpressionBinder(query_context), bound_select_statement_(bound_select_statement) {}

    // Bind expression entry
    std::shared_ptr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    std::shared_ptr<BaseExpression> BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    std::shared_ptr<BaseExpression> BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    [[nodiscard]] const std::string &BoundColumn() const { return bound_column_name_; }

    std::shared_ptr<BaseExpression> BuildUnnestExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

private:
    bool binding_agg_func_ = false;
    std::string bound_column_name_;

    BoundSelectStatement *bound_select_statement_ = nullptr;
};

} // namespace infinity
