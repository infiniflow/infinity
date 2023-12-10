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

import bound_statement;
import table_ref;
import base_expression;
import bind_context;
import logical_node;
import query_context;
import stl;

export module bound_update_statement;

namespace infinity {

// class BindContext;
// class QueryContext;
// class BaseExpression;

export struct BoundUpdateStatement final : public BoundStatement {
public:
    static inline UniquePtr<BoundUpdateStatement> Make(SharedPtr<BindContext> bind_context) {
        return MakeUnique<BoundUpdateStatement>(Move(bind_context));
    }

public:
    inline explicit BoundUpdateStatement(SharedPtr<BindContext> bind_context) : bind_context_(Move(bind_context)) {}

    SharedPtr<LogicalNode> BuildPlan(QueryContext *query_context) final;

    SharedPtr<LogicalNode> BuildFrom(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context);

    SharedPtr<LogicalNode> BuildBaseTable(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context);

    SharedPtr<LogicalNode> BuildFilter(SharedPtr<LogicalNode> &root,
                                       Vector<SharedPtr<BaseExpression>> &conditions,
                                       QueryContext *query_context,
                                       const SharedPtr<BindContext> &bind_context);

    void BuildSubquery(SharedPtr<LogicalNode> &root,
                       SharedPtr<BaseExpression> &condition,
                       QueryContext *query_context,
                       const SharedPtr<BindContext> &bind_context);

    SharedPtr<BaseExpression> UnnestSubquery(SharedPtr<LogicalNode> &root,
                                             SharedPtr<BaseExpression> &condition,
                                             QueryContext *query_context,
                                             const SharedPtr<BindContext> &bind_context);

public:
    SharedPtr<BindContext> bind_context_{};

    // From clause
    SharedPtr<TableRef> table_ref_ptr_{};

    // Where conditions
    Vector<SharedPtr<BaseExpression>> where_conditions_{};

    Vector<Pair<SizeT, SharedPtr<BaseExpression>>> update_columns_{};

    // For build subquery
    bool building_subquery_{false};
};

} // namespace infinity
