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

module join_binder;

import stl;
import base_expression;

import bind_context;
import status;
import infinity_exception;
import parsed_expr;
import knn_expr;
import logger;

namespace infinity {

SharedPtr<BaseExpression> JoinBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result;
    switch (expr.type_) {
        case ParsedExprType::kSubquery: {
            Status status = Status::SyntaxError("Subquery isn't allowed in JOIN condition.");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        default: {
            result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
        }
    }
    return result;
}

SharedPtr<BaseExpression> JoinBinder::BuildKnnExpr(const KnnExpr &, BindContext *, i64 , bool ) {
    RecoverableError(Status::SyntaxError("KNN expression isn't supported in join clause."));
    return nullptr;
}

} // namespace infinity
