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

module bind_alias_proxy;

import stl;
import base_expression;

import bind_context;
import expression_binder;
import status;
import infinity_exception;
import third_party;
import parsed_expr;
import logger;

namespace infinity {

SharedPtr<BaseExpression>
BindAliasProxy::BindAlias(ExpressionBinder &expression_binder, const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    String expr_name = expr.GetName();

    auto alias_pair = bind_context_ptr->select_alias2index_.find(expr_name);
    if (alias_pair == bind_context_ptr->select_alias2index_.end()) {
        return nullptr;
    }

    const ParsedExpr *select_expr = bind_context_ptr->select_expression_[alias_pair->second];

    if (binding_alias_) {
        Status status = Status::SyntaxError(fmt::format("Trying to bind an alias table_name: {} in another alias", expr_name));
        RecoverableError(status);
    }

    binding_alias_ = true;
    SharedPtr<BaseExpression> bound_alias_expr = expression_binder.BuildExpression(*select_expr, bind_context_ptr, depth, root);
    binding_alias_ = false;

    return bound_alias_expr;
}

} // namespace infinity
