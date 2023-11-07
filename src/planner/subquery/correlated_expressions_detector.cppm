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
import logical_node_visitor;
import column_expression;
import base_expression;
import logical_node;
import subquery_expression;

export module corrlated_expr_detector;

namespace infinity {

export class CorrelatedExpressionsDetector final : public LogicalNodeVisitor {
public:
    explicit CorrelatedExpressionsDetector(const Vector<SharedPtr<ColumnExpression>> &correlated_column_exprs)
        : correlated_column_exprs_(correlated_column_exprs) {}

    void VisitNode(LogicalNode &op) final;

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) override;

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<SubqueryExpression> &expression) override;

    [[nodiscard]] inline bool IsCorrelated() const { return is_correlated_; }

private:
    const Vector<SharedPtr<ColumnExpression>> &correlated_column_exprs_;
    bool is_correlated_{false};
};

} // namespace infinity
