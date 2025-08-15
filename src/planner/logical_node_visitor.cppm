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

export module infinity_core:logical_node_visitor;

import :base_expression;
import :logical_node;
import :aggregate_expression;
import :between_expression;
import :case_expression;
import :cast_expression;
import :column_expression;
import :function_expression;
import :value_expression;
import :in_expression;
import :subquery_expression;
import :knn_expression;
import :conjunction_expression;
import :unnest_expression;

namespace infinity {

export class LogicalNodeVisitor {
public:
    virtual void VisitNode(LogicalNode &op) = 0;

    virtual ~LogicalNodeVisitor() = default;

    void VisitNodeChildren(LogicalNode &op);

    void VisitNodeExpression(LogicalNode &op);

    void VisitExpression(std::shared_ptr<BaseExpression> &expression);

    void VisitExpressionChildren(std::shared_ptr<BaseExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<AggregateExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<BetweenExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<CaseExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<CastExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<ColumnExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<ConjunctionExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<FunctionExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<ValueExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<InExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<SubqueryExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<KnnExpression> &expression);

    virtual std::shared_ptr<BaseExpression> VisitReplace(const std::shared_ptr<UnnestExpression> &expression);
};

} // namespace infinity
