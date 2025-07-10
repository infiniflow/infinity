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

export module bound_insert_statement;

import bound_statement;
import table_ref;
import base_expression;
import bind_context;
import logical_node;
import query_context;
import stl;

namespace infinity {

export struct BoundInsertStatement final : public BoundStatement {
public:
    static inline UniquePtr<BoundInsertStatement> Make(SharedPtr<BindContext> bind_context) {
        return MakeUnique<BoundInsertStatement>(std::move(bind_context));
    }

public:
    inline explicit BoundInsertStatement(SharedPtr<BindContext> bind_context) : bind_context_(std::move(bind_context)) {}

    SharedPtr<LogicalNode> BuildPlan(QueryContext *query_context) final;

    SharedPtr<LogicalNode> BuildFrom(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context);

    SharedPtr<LogicalNode> BuildBaseTable(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context);

public:
    SharedPtr<BindContext> bind_context_{};

    // Target table for insertion
    SharedPtr<TableRef> table_ref_ptr_{};

    // Column names for insertion (if specified)
    Vector<String> columns_for_select_{};

    // SELECT statement to get data from
    SharedPtr<LogicalNode> select_plan_{};

    // Value lists for direct value insertion (alternative to select_plan_)
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_{};
};

} // namespace infinity
