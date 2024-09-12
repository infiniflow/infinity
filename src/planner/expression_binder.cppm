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

export module expression_binder;

import stl;
import function;
import bind_context;
import base_expression;
import query_context;
import subquery_expression;
import parsed_expr;
import column_expr;
import constant_expr;
import between_expr;
import function_expr;
import case_expr;
import in_expr;
import knn_expr;
import match_expr;
import match_tensor_expr;
import match_sparse_expr;
import search_expr;
import subquery_expr;
import cast_expr;

namespace infinity {

struct ExprBindResult {
    SharedPtr<BaseExpression> bound_expression_ptr_{};
    String err_msg_{};

    inline bool IsOk() { return err_msg_.empty(); }
};

export class ExpressionBinder {
public:
    explicit ExpressionBinder(QueryContext *query_context) : query_context_(query_context) {}

    virtual ~ExpressionBinder() = default;

    SharedPtr<BaseExpression> Bind(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    // Bind expression entry
    virtual SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildBetweenExpr(const BetweenExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildValueExpr(const ConstantExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    // Bind column reference expression also include correlated column reference.
    virtual SharedPtr<BaseExpression> BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual void CheckFuncType(FunctionType) const {}

    virtual SharedPtr<BaseExpression> BuildCastExpr(const CastExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildCaseExpr(const CaseExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildInExpr(const InExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildMatchTextExpr(const MatchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildMatchTensorExpr(const MatchTensorExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildMatchSparseExpr(const MatchSparseExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual SharedPtr<BaseExpression> BuildSearchExpr(const SearchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    // Bind subquery expression.
    virtual SharedPtr<SubqueryExpression>
    BuildSubquery(const SubqueryExpr &expr, BindContext *bind_context_ptr, SubqueryType subquery_type, i64 depth, bool root);

    //    // Bind window function.
    //    virtual SharedPtr<BaseExpression>
    //    BuildWindow(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    //    SharedPtr<PlanBuilder> plan_builder_ptr_;
protected:
    Optional<SharedPtr<BaseExpression>> TryBuildSpecialFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth);

    QueryContext *query_context_{};
};

} // namespace infinity
