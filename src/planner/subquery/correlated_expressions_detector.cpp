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
import subquery_expression;

import infinity_exception;

module corrlated_expr_detector;

namespace infinity {

void CorrelatedExpressionsDetector::VisitNode(LogicalNode &op) { VisitNodeExpression(op); }

SharedPtr<BaseExpression> CorrelatedExpressionsDetector::VisitReplace(const SharedPtr<ColumnExpression> &expression) {

    if (expression->depth() == 0) {
        return expression;
    }

    if (expression->depth() > 1) {
        Error<PlannerException>("Column expression with depth > 1 is detected");
    }

    is_correlated_ = true;
    return expression;
}

SharedPtr<BaseExpression> CorrelatedExpressionsDetector::VisitReplace(const SharedPtr<SubqueryExpression> &expression) {
    if (expression->correlated_columns.empty()) {
        // Uncorrelated subquery
        return nullptr;
    }

    Error<PlannerException>("Not support nested correlated subquery in the subquery plan");
}

} // namespace infinity
