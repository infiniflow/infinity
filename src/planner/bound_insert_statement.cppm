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

export module infinity_core:bound_insert_statement;

import :bound_statement;
import :table_ref;
import :base_expression;
import :bind_context;
import :logical_node;
import :query_context;
import :stl;

namespace infinity {

export struct BoundInsertStatement final : public BoundStatement {
public:
    static inline std::unique_ptr<BoundInsertStatement> Make(std::shared_ptr<BindContext> bind_context) {
        return std::make_unique<BoundInsertStatement>(std::move(bind_context));
    }

public:
    inline explicit BoundInsertStatement(std::shared_ptr<BindContext> bind_context) : bind_context_(std::move(bind_context)) {}

    std::shared_ptr<LogicalNode> BuildPlan(QueryContext *query_context) final;

    std::shared_ptr<LogicalNode> BuildFrom(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode> BuildBaseTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

public:
    std::shared_ptr<BindContext> bind_context_{};

    // Target table for insertion
    std::shared_ptr<TableRef> table_ref_ptr_{};

    // Column names for insertion (if specified)
    std::vector<std::string> columns_for_select_{};

    // SELECT statement to get data from
    std::shared_ptr<LogicalNode> select_plan_{};

    // Value lists for direct value insertion (alternative to select_plan_)
    std::vector<std::vector<std::shared_ptr<BaseExpression>>> value_list_{};
};

} // namespace infinity
