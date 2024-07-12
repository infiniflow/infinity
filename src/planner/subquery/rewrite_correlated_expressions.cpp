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

module rewrite_correlated_expression;

import stl;
import logical_node;
import base_expression;
import column_expression;
import status;
import infinity_exception;
import subquery_expression;
import logger;

namespace infinity {

void RewriteCorrelatedExpressions::VisitNode(LogicalNode &op) { VisitNodeExpression(op); }

SharedPtr<BaseExpression> RewriteCorrelatedExpressions::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    if (expression->depth() == 0) {
        return expression;
    }

    if (expression->depth() > 1) {
        Status status = Status::SyntaxError("Correlated depth > 1 is not supported now.");
        RecoverableError(status);
    }

    auto entry = bind_context_ptr_->correlated_column_map_.find(expression->binding());
    if (entry == bind_context_ptr_->correlated_column_map_.end()) {
        // This column expression wasn't stored in correlated column map before
        String error_message = "Correlated expression isn't found.";
        UnrecoverableError(error_message);
    }

    expression->SetBinding(base_binding_.table_idx, base_binding_.column_idx + entry->second);
    expression->SetDepth(0);

    return expression;
}

SharedPtr<BaseExpression> RewriteCorrelatedExpressions::VisitReplace(const SharedPtr<SubqueryExpression> &expression) {
    if (expression->correlated_columns.empty()) {
        // Not correlated
        return nullptr;
    }

    Status status = Status::SyntaxError("Not support rewrite nested correlated subquery in the subquery plan");
    RecoverableError(status);
    return nullptr;
}

} // namespace infinity
