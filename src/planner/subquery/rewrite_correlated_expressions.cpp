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
import logical_node;
import base_expression;
import column_expression;

import infinity_exception;
import subquery_expression;

module rewrite_correlated_expression;

namespace infinity {

void RewriteCorrelatedExpressions::VisitNode(LogicalNode &op) { VisitNodeExpression(op); }

SharedPtr<BaseExpression> RewriteCorrelatedExpressions::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    if (expression->depth() == 0) {
        return expression;
    }

    if (expression->depth() > 1) {
        Error<PlannerException>("Correlated depth > 1 is not suppported now.");
    }

    auto entry = bind_context_ptr_->correlated_column_map_.find(expression->binding());
    if (entry == bind_context_ptr_->correlated_column_map_.end()) {
        // This column expression wasn't stored in correlated column map before
        Error<PlannerException>("Correlated expression isn't found.");
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

    Error<PlannerException>("Not support rewrite nested correlated subquery in the subquery plan");
    return nullptr;
}

} // namespace infinity
