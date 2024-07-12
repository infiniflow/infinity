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

module where_binder;

import stl;
import base_expression;

import function;
import bind_context;

import infinity_exception;
import third_party;
import bind_alias_proxy;
import parsed_expr;
import column_expr;
import status;
import logger;

namespace infinity {

SharedPtr<BaseExpression> WhereBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    return result;
}

SharedPtr<BaseExpression> WhereBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    // Step 1. Trying to bind the column to current binding. (done)
    // Step 2. Trying to bind the column to current select list alias. (done)

    // Use expression binder to check the column firstly
    auto result = ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);

    if (result.get() != nullptr) {
        return result;
    }

    // The column isn't found, check if it is an alias of select list expression
    if (bind_alias_proxy_.get() != nullptr) {
        // Yes, it is an alias from select list expression
        // Extract the expression, and bind it during binding where clause phase .
        result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr, depth, root);
    }

    if (result.get() == nullptr) {
        Status status = Status::ColumnNotExist(expr.GetName());
        RecoverableError(status);
    }
    return result;
}

void WhereBinder::CheckFuncType(FunctionType func_type) const {
    if (func_type != FunctionType::kScalar) {
        String error_message = "Only scalar function are allowed in where clause";
        UnrecoverableError(error_message);
    }

}

} // namespace infinity