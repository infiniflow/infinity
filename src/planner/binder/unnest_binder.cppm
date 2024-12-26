// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module unnest_binder;

import stl;

import base_expression;
import bind_context;
import expression_binder;
import query_context;
import parsed_expr;
import knn_expr;
import match_expr;
import match_tensor_expr;
import match_sparse_expr;
import search_expr;

namespace infinity {

export class UnnestBinder final : public ExpressionBinder {
public:
    explicit UnnestBinder(QueryContext *query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildMatchTextExpr(const MatchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildMatchTensorExpr(const MatchTensorExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildMatchSparseExpr(MatchSparseExpr &&expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildSearchExpr(const SearchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;
};

} // namespace infinity
