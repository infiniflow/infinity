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

export module infinity_core:expression_binder;

import :function;
import :bind_context;
import :base_expression;
import :subquery_expression;

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
    std::shared_ptr<BaseExpression> bound_expression_ptr_{};
    std::string err_msg_{};

    inline bool IsOk() { return err_msg_.empty(); }
};

export class ExpressionBinder {
public:
    explicit ExpressionBinder(QueryContext *query_context) : query_context_(query_context) {}

    virtual ~ExpressionBinder() = default;

    std::shared_ptr<BaseExpression> Bind(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    // Bind expression entry
    virtual std::shared_ptr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildBetweenExpr(const BetweenExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildValueExpr(const ConstantExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    // Bind column reference expression also include correlated column reference.
    virtual std::shared_ptr<BaseExpression> BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual void CheckFuncType(FunctionType) const {}

    virtual std::shared_ptr<BaseExpression> BuildCastExpr(const CastExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildCaseExpr(const CaseExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildInExpr(const InExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildMatchTextExpr(const MatchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildMatchTensorExpr(const MatchTensorExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildMatchSparseExpr(MatchSparseExpr &&expr, BindContext *bind_context_ptr, i64 depth, bool root);

    virtual std::shared_ptr<BaseExpression> BuildSearchExpr(const SearchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

    // Bind subquery expression.
    virtual std::shared_ptr<SubqueryExpression>
    BuildSubquery(const SubqueryExpr &expr, BindContext *bind_context_ptr, SubqueryType subquery_type, i64 depth, bool root);

    //    // Bind window function.
    //    virtual std::shared_ptr<BaseExpression>
    //    BuildWindow(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    //    std::shared_ptr<PlanBuilder> plan_builder_ptr_;

    virtual std::shared_ptr<BaseExpression> BuildUnnestExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root);

protected:
    std::optional<std::shared_ptr<BaseExpression>> TryBuildSpecialFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth);

    static bool IsUnnestedFunction(const std::string &function_name);

    QueryContext *query_context_{};
};

} // namespace infinity
