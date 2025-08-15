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

export module infinity_core:aggregate_binder;

import :stl;

import :base_expression;
import :bind_context;
import :expression_binder;

import parsed_expr;
import knn_expr;

namespace infinity {

class AggregateBinder : public ExpressionBinder {
public:
    //    explicit AggregateBinder(PlanBuilder& plan_builder) : ExpressionBinder(plan_builder) {}

    // Bind expression entry
    std::shared_ptr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

    //    // Bind column reference expression also include correlated column reference.
    //    std::shared_ptr<BaseExpression>
    //    BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<BaseExpression> BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

private:
};

} // namespace infinity
