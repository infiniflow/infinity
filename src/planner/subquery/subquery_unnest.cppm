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

export module infinity_core:subquery_unnest;

import :column_binding;
import :logical_node;
import :base_expression;
import :subquery_expression;
import :column_expression;

namespace infinity {

class QueryContext;
class BindContext;

export class SubqueryUnnest {
public:
    static void UnnestSubqueries(std::shared_ptr<BaseExpression> &expr_ptr,
                                 std::shared_ptr<LogicalNode> &root,
                                 QueryContext *query_context,
                                 const std::shared_ptr<BindContext> &bind_context);

    static std::shared_ptr<BaseExpression> UnnestSubquery(std::shared_ptr<BaseExpression> &expr_ptr,
                                                    std::shared_ptr<LogicalNode> &root,
                                                    QueryContext *query_context,
                                                    const std::shared_ptr<BindContext> &bind_context);

    static std::shared_ptr<BaseExpression> UnnestUncorrelated(SubqueryExpression *expr_ptr,
                                                        std::shared_ptr<LogicalNode> &root,
                                                        std::shared_ptr<LogicalNode> &subquery_plan,
                                                        QueryContext *query_context,
                                                        const std::shared_ptr<BindContext> &bind_context);

    static std::shared_ptr<BaseExpression> UnnestCorrelated(SubqueryExpression *expr_ptr,
                                                      std::shared_ptr<LogicalNode> &root,
                                                      std::shared_ptr<LogicalNode> &subquery_plan,
                                                      QueryContext *query_context,
                                                      const std::shared_ptr<BindContext> &bind_context);

private:
    static void GenerateJoinConditions(QueryContext *query_context,
                                       std::vector<std::shared_ptr<BaseExpression>> &conditions,
                                       const std::vector<std::shared_ptr<ColumnExpression>> &correlated_columns,
                                       const std::vector<ColumnBinding> &subplan_column_bindings,
                                       size_t correlated_base_index);
};

} // namespace infinity
