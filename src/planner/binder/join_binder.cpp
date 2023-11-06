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

import infinity_exception;

module join_binder;

namespace infinity {

SharedPtr<BaseExpression> JoinBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result;
    switch (expr.type_) {
        case ParsedExprType::kSubquery: {
            Error<PlannerException>("Subquery isn't allowed in JOIN condition.");
        }
        default: {
            result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
        }
    }
    return result;
}

SharedPtr<BaseExpression> JoinBinder::BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Error<PlannerException>("KNN expression isn't supported in join clause.");
}

} // namespace infinity
