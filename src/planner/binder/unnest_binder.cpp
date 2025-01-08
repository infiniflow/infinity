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

module unnest_binder;

import stl;
import base_expression;

import bind_context;
import column_expression;
import function;
import status;
import infinity_exception;
import third_party;
import function_set;
import bind_alias_proxy;
import logger;

namespace infinity {

SharedPtr<BaseExpression> UnnestBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    return result;
}

SharedPtr<BaseExpression> UnnestBinder::BuildKnnExpr(const KnnExpr &, BindContext *, i64, bool) {
    Status status = Status::SyntaxError("Match vector expression isn't supported in unnest clause");
    RecoverableError(status);
    return nullptr;
}

SharedPtr<BaseExpression> UnnestBinder::BuildMatchTextExpr(const MatchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Status status = Status::SyntaxError("Match text expression isn't supported in unnest clause");
    RecoverableError(status);
    return nullptr;
}

SharedPtr<BaseExpression> UnnestBinder::BuildMatchTensorExpr(const MatchTensorExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Status status = Status::SyntaxError("Match tensor expression isn't supported in unnest clause");
    RecoverableError(status);
    return nullptr;
}

SharedPtr<BaseExpression> UnnestBinder::BuildMatchSparseExpr(MatchSparseExpr &&expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Status status = Status::SyntaxError("Match sparse expression isn't supported in unnest clause");
    RecoverableError(status);
    return nullptr;
}

SharedPtr<BaseExpression> UnnestBinder::BuildSearchExpr(const SearchExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Status status = Status::SyntaxError("Search expression isn't supported in unnest clause");
    RecoverableError(status);
    return nullptr;
}

} // namespace infinity