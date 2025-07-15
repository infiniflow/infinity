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

export module infinity_core:order_binder;

import :stl;

import :base_expression;
import :bind_context;
import :expression_binder;
// import :query_context;
import parsed_expr;

namespace infinity {

export class OrderBinder : public ExpressionBinder {
public:
    static void PushExtraExprToSelectList(ParsedExpr *expr, const SharedPtr<BindContext> &bind_context_ptr);

public:
    explicit OrderBinder(QueryContext *query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression> BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) override;

private:
};

} // namespace infinity
