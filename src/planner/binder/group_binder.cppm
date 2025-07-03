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

export module group_binder;

import stl;

import base_expression;
import subquery_expression;
import bind_context;
import expression_binder;
import query_context;
import function;
import bind_alias_proxy;
import parsed_expr;
import column_expr;
import constant_expr;
import function_expr;
import knn_expr;
import subquery_expr;

namespace infinity {

export class GroupBinder : public ExpressionBinder {
public:
    i64 group_by_expr_index{-1};

    HashSet<i64> bound_select_index_;

public:
    explicit GroupBinder(QueryContext *query_context, const SharedPtr<BindAliasProxy> &bind_alias_proxy)
        : ExpressionBinder(query_context), bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

    SharedPtr<BaseExpression> BindColumnReference(const ColumnExpr &expr, BindContext *bind_context_ptr);

    SharedPtr<BaseExpression> BindConstantExpression(const ConstantExpr &expr, BindContext *bind_context_ptr);

    SharedPtr<BaseExpression> BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

    SharedPtr<BaseExpression> BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

    void CheckFuncType(FunctionType func_type) const override;

    SharedPtr<SubqueryExpression>
    BuildSubquery(const SubqueryExpr &select, BindContext *bind_context_ptr, SubqueryType subquery_type, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

private:
    const SharedPtr<BindAliasProxy> &bind_alias_proxy_;
};

} // namespace infinity
