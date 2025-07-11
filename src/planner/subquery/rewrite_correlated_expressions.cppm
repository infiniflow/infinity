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

export module infinity_core:rewrite_correlated_expression;

import :stl;
import :logical_node_visitor;
import :logical_node;
import :column_binding;
import :base_expression;
import :column_expression;
import :subquery_expression;

namespace infinity {

class BindContext;

export class RewriteCorrelatedExpressions : public LogicalNodeVisitor {
public:
    explicit RewriteCorrelatedExpressions(const SharedPtr<BindContext> &bind_context_ptr, ColumnBinding base_binding)
        : bind_context_ptr_(bind_context_ptr), base_binding_(base_binding) {}

    void VisitNode(LogicalNode &op) final;

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression) override;

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<SubqueryExpression> &expression) override;

private:
    const SharedPtr<BindContext> &bind_context_ptr_;
    ColumnBinding base_binding_{};
};

} // namespace infinity
