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

module infinity_core:rewrite_correlated_expressions.impl;

import :rewrite_correlated_expression;
import :logical_node;
import :base_expression;
import :column_expression;
import :status;
import :infinity_exception;
import :subquery_expression;

namespace infinity {

void RewriteCorrelatedExpressions::VisitNode(LogicalNode &op) { VisitNodeExpression(op); }

std::shared_ptr<BaseExpression> RewriteCorrelatedExpressions::VisitReplace(const std::shared_ptr<ColumnExpression> &expression) {
    if (expression->depth() == 0) {
        return expression;
    }

    if (expression->depth() > 1) {
        RecoverableError(Status::SyntaxError("Correlated depth > 1 is not supported now."));
    }

    auto entry = bind_context_ptr_->correlated_column_map_.find(expression->binding());
    if (entry == bind_context_ptr_->correlated_column_map_.end()) {
        // This column expression wasn't stored in correlated column map before
        UnrecoverableError("Correlated expression isn't found.");
    }

    expression->SetBinding(base_binding_.table_idx, base_binding_.column_idx + entry->second);
    expression->SetDepth(0);

    return expression;
}

std::shared_ptr<BaseExpression> RewriteCorrelatedExpressions::VisitReplace(const std::shared_ptr<SubqueryExpression> &expression) {
    if (expression->correlated_columns.empty()) {
        // Not correlated
        return nullptr;
    }

    RecoverableError(Status::SyntaxError("Not support rewrite nested correlated subquery in the subquery plan"));
    return nullptr;
}

} // namespace infinity
