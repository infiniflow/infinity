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

export module logical_node_visitor;

import stl;
import base_expression;
import logical_node;
import aggregate_expression;
import between_expression;
import case_expression;
import cast_expression;
import column_expression;
import function_expression;
import value_expression;
import in_expression;
import subquery_expression;
import knn_expression;
import conjunction_expression;
import unnest_expression;

namespace infinity {

export class LogicalNodeVisitor {
public:
    virtual void VisitNode(LogicalNode &op) = 0;

    virtual ~LogicalNodeVisitor() = default;

    void VisitNodeChildren(LogicalNode &op);

    void VisitNodeExpression(LogicalNode &op);

    void VisitExpression(SharedPtr<BaseExpression> &expression);

    void VisitExpressionChildren(SharedPtr<BaseExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<AggregateExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<BetweenExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<CaseExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<CastExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ColumnExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ConjunctionExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<FunctionExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<ValueExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<InExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<SubqueryExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<KnnExpression> &expression);

    virtual SharedPtr<BaseExpression> VisitReplace(const SharedPtr<UnnestExpression> &expression);
};

} // namespace infinity
