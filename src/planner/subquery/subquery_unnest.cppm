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

export module infinity_core:subquery_unnest;

import :stl;
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
    static void UnnestSubqueries(SharedPtr<BaseExpression> &expr_ptr,
                                 SharedPtr<LogicalNode> &root,
                                 QueryContext *query_context,
                                 const SharedPtr<BindContext> &bind_context);

    static SharedPtr<BaseExpression> UnnestSubquery(SharedPtr<BaseExpression> &expr_ptr,
                                                    SharedPtr<LogicalNode> &root,
                                                    QueryContext *query_context,
                                                    const SharedPtr<BindContext> &bind_context);

    static SharedPtr<BaseExpression> UnnestUncorrelated(SubqueryExpression *expr_ptr,
                                                        SharedPtr<LogicalNode> &root,
                                                        SharedPtr<LogicalNode> &subquery_plan,
                                                        QueryContext *query_context,
                                                        const SharedPtr<BindContext> &bind_context);

    static SharedPtr<BaseExpression> UnnestCorrelated(SubqueryExpression *expr_ptr,
                                                      SharedPtr<LogicalNode> &root,
                                                      SharedPtr<LogicalNode> &subquery_plan,
                                                      QueryContext *query_context,
                                                      const SharedPtr<BindContext> &bind_context);

private:
    static void GenerateJoinConditions(QueryContext *query_context,
                                       Vector<SharedPtr<BaseExpression>> &conditions,
                                       const Vector<SharedPtr<ColumnExpression>> &correlated_columns,
                                       const Vector<ColumnBinding> &subplan_column_bindings,
                                       SizeT correlated_base_index);
};

} // namespace infinity
