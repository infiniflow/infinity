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

export module infinity_core:bound_update_statement;

import :bound_statement;
import :table_ref;
import :base_expression;
import :bind_context;
import :logical_node;

namespace infinity {

export struct BoundUpdateStatement final : public BoundStatement {
public:
    static inline std::unique_ptr<BoundUpdateStatement> Make(std::shared_ptr<BindContext> bind_context) {
        return std::make_unique<BoundUpdateStatement>(std::move(bind_context));
    }

public:
    inline explicit BoundUpdateStatement(std::shared_ptr<BindContext> bind_context) : bind_context_(std::move(bind_context)) {}

    std::shared_ptr<LogicalNode> BuildPlan(QueryContext *query_context) final;

    std::shared_ptr<LogicalNode> BuildFrom(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode> BuildBaseTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode> BuildFilter(std::shared_ptr<LogicalNode> &root,
                                       std::vector<std::shared_ptr<BaseExpression>> &conditions,
                                       QueryContext *query_context,
                                       const std::shared_ptr<BindContext> &bind_context);

    void BuildSubquery(std::shared_ptr<LogicalNode> &root,
                       std::shared_ptr<BaseExpression> &condition,
                       QueryContext *query_context,
                       const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<BaseExpression> UnnestSubquery(std::shared_ptr<LogicalNode> &root,
                                             std::shared_ptr<BaseExpression> &condition,
                                             QueryContext *query_context,
                                             const std::shared_ptr<BindContext> &bind_context);

public:
    std::shared_ptr<BindContext> bind_context_{};

    // From clause
    std::shared_ptr<TableRef> table_ref_ptr_{};

    // Where conditions
    std::vector<std::shared_ptr<BaseExpression>> where_conditions_{};

    std::vector<std::pair<size_t, std::shared_ptr<BaseExpression>>> update_columns_{};
    std::vector<std::shared_ptr<BaseExpression>> all_columns_in_table_{};
    std::vector<std::shared_ptr<BaseExpression>> final_result_columns_{};

    // For build subquery
    bool building_subquery_{false};
};

} // namespace infinity
