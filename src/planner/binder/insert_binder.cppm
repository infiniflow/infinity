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
import parser;
import base_expression;
import bind_context;
import expression_binder;
import query_context;

export module insert_binder;

namespace infinity {

export class InsertBinder final : public ExpressionBinder {
public:
    explicit InsertBinder(QueryContext *query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) final;

    SharedPtr<BaseExpression> BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

    //    // Bind column reference expression also include correlated column reference.
    //    SharedPtr<BaseExpression>
    //    BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;
private:
};

} // namespace infinity
