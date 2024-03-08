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

module limit_binder;

import stl;
import base_expression;

import bind_context;
import function_set;
import function;
import status;
import infinity_exception;

namespace infinity {

SharedPtr<BaseExpression> LimitBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    switch (expr.type_) {
        case ParsedExprType::kParameter: {
            RecoverableError(Status::SyntaxError("Parameter expression isn't allowed in limit expression."));
        }
        case ParsedExprType::kSubquery: {
            RecoverableError(Status::SyntaxError("Subquery expression isn't allowed in limit expression."));
        }
        default: {
            return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
        }
    }
}

SharedPtr<BaseExpression> LimitBinder::BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), expr);
    if (function_set_ptr->type_ != FunctionType::kScalar) {
        RecoverableError(Status::SyntaxError("Only scalar function is supported in limit clause."));
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);
}

SharedPtr<BaseExpression> LimitBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    if (expr.star_) {
        RecoverableError(Status::SyntaxError("Star expression isn't allowed in limit clause."));
    }
    return ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
}

SharedPtr<BaseExpression> LimitBinder::BuildKnnExpr(const KnnExpr &, BindContext *, i64, bool) {
    RecoverableError(Status::SyntaxError("KNN expression isn't supported in limit clause"));
    return nullptr;
}

} // namespace infinity
