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

export module infinity_core:bound_select_statement;

import :bound_statement;
import :logical_knn_scan;
import :base_expression;
import :bind_context;
import :logical_node;
import :table_ref;
import :search_expression;
import :knn_expression;
import :highlighter;

import select_statement;

namespace infinity {

export struct BoundSelectStatement final : public BoundStatement {
public:
    static inline std::unique_ptr<BoundSelectStatement> Make(std::shared_ptr<BindContext> bind_context) {
        return std::make_unique<BoundSelectStatement>(std::move(bind_context));
    }

public:
    inline explicit BoundSelectStatement(std::shared_ptr<BindContext> bind_context) : bind_context_(std::move(bind_context)) {}

    std::shared_ptr<LogicalNode> BuildPlan(QueryContext *query_context) final;

    std::shared_ptr<LogicalKnnScan> BuildInitialKnnScan(std::shared_ptr<TableRef> &table_ref,
                                                        std::shared_ptr<KnnExpression> knn_expr,
                                                        QueryContext *query_context,
                                                        const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode>
    BuildFrom(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode>
    BuildBaseTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode>
    BuildSubqueryTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode>
    BuildCrossProductTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode>
    BuildJoinTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode>
    BuildDummyTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode> BuildFilter(std::shared_ptr<LogicalNode> &root,
                                             std::vector<std::shared_ptr<BaseExpression>> &conditions,
                                             QueryContext *query_context,
                                             const std::shared_ptr<BindContext> &bind_context);

    std::shared_ptr<LogicalNode> BuildUnnest(std::shared_ptr<LogicalNode> &root,
                                             std::vector<std::shared_ptr<BaseExpression>> &expressions,
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

    // Search expression
    std::shared_ptr<SearchExpression> search_expr_{};

    // Where conditions
    std::vector<std::shared_ptr<BaseExpression>> where_conditions_{};

    // Aggregate expression
    std::vector<std::shared_ptr<BaseExpression>> aggregate_expressions_{};

    // Group by expression list
    std::vector<std::shared_ptr<BaseExpression>> group_by_expressions_{};

    // Having expression list
    std::vector<std::shared_ptr<BaseExpression>> having_expressions_{};

    // Project expression list
    std::vector<std::shared_ptr<BaseExpression>> projection_expressions_{};
    bool total_hits_count_flag_{false};

    // Highlight info
    std::map<size_t, std::shared_ptr<HighlightInfo>> highlight_columns_{};

    // Unnest columns
    std::vector<std::shared_ptr<BaseExpression>> unnest_expressions_{};

    // Order by expression list
    std::vector<std::shared_ptr<BaseExpression>> order_by_expressions_{};
    std::vector<OrderType> order_by_types_{};

    // limit expression
    std::shared_ptr<BaseExpression> limit_expression_{};
    std::shared_ptr<BaseExpression> offset_expression_{};

    // Pruned output expression list
    std::vector<std::shared_ptr<BaseExpression>> pruned_expression_{};

    bool distinct_ = false;

    //    std::shared_ptr<LogicalNode>
    //    BuildPlan() override {
    //        return this->logical_plan_;
    //    }

    u64 groupby_index_{0};
    u64 aggregate_index_{0};
    u64 projection_index_{0};
    u64 result_index_{0};
    u64 knn_index_{0};

    // For build subquery
    bool building_subquery_{false};
};

} // namespace infinity
