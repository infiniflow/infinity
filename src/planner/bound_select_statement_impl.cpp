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

#include <cstdlib>
#include <string>

module infinity_core:bound_select_statement.impl;

import :bound_select_statement;

import :logical_node;
import :stl;
import :query_context;
import :bind_context;
import :table_ref;
import :base_expression;
import :conjunction_expression;
import :subquery_expression;

import :logical_node;
import :logical_node_type;
import :logical_create_schema;
import :logical_create_view;
import :logical_create_table;
import :logical_create_collection;
import :logical_drop_schema;
import :logical_drop_table;
import :logical_drop_collection;
import :logical_drop_view;
import :logical_flush;
import :logical_insert;
import :logical_project;
import :logical_filter;
import :logical_table_scan;
import :logical_knn_scan;
import :logical_match_tensor_scan;
import :logical_match_sparse_scan;
import :logical_aggregate;
import :logical_sort;
import :logical_limit;
import :logical_top;
import :logical_cross_product;
import :logical_join;
import :logical_show;
import :logical_export;
import :logical_import;
import :logical_dummy_scan;
import :logical_match;
import :logical_fusion;
import :logical_unnest;

import :subquery_unnest;

import :infinity_exception;
import :expression_transformer;
import :expression_type;

import :base_table_ref;
import :subquery_table_ref;
import :cross_product_table_ref;
import :join_table_ref;
import :knn_expression;
import :match_expression;
import :match_tensor_expression;
import :match_sparse_expression;
import :third_party;
import table_reference;
import :common_query_filter;
import :logger;

import search_options;
import :search_driver;
import :query_node;
import :doc_iterator;
import :status;
import :default_values;
import :parse_fulltext_options;
import :highlighter;
import data_type;
import internal_types;
import :new_txn;

namespace infinity {

SharedPtr<LogicalNode> BoundSelectStatement::BuildPlan(QueryContext *query_context) {
    const SharedPtr<BindContext> &bind_context = this->bind_context_;
    if (search_expr_.get() == nullptr) {
        SharedPtr<LogicalNode> root = BuildFrom(table_ref_ptr_, query_context, bind_context);

        if (!unnest_expressions_.empty()) {
            SharedPtr<LogicalNode> unnest = BuildUnnest(root, unnest_expressions_, query_context, bind_context);
            unnest->set_left_node(root);
            root = unnest;
        }

        if (!where_conditions_.empty()) {
            SharedPtr<LogicalNode> filter = BuildFilter(root, where_conditions_, query_context, bind_context);
            filter->set_left_node(root);
            root = filter;
        }

        if (!group_by_expressions_.empty() || !aggregate_expressions_.empty()) {
            // Build logical aggregate
            auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref_ptr_);
            auto aggregate = MakeShared<LogicalAggregate>(bind_context->GetNewLogicalNodeId(),
                                                          base_table_ref,
                                                          group_by_expressions_,
                                                          groupby_index_,
                                                          aggregate_expressions_,
                                                          aggregate_index_);
            aggregate->set_left_node(root);
            root = aggregate;
        }

        if (!having_expressions_.empty()) {
            // Build logical filter
            auto having_filter = BuildFilter(root, having_expressions_, query_context, bind_context);
            having_filter->set_left_node(root);
            root = having_filter;
        }

        if (!order_by_expressions_.empty()) {
            if (order_by_expressions_.size() != order_by_types_.size()) {
                String error_message = "Unknown error on order by expression";
                UnrecoverableError(error_message);
            }

            if (limit_expression_.get() == nullptr) {
                SharedPtr<LogicalNode> sort = MakeShared<LogicalSort>(bind_context->GetNewLogicalNodeId(), order_by_expressions_, order_by_types_);
                sort->set_left_node(root);
                root = sort;
            } else {
                SharedPtr<LogicalNode> top = MakeShared<LogicalTop>(bind_context->GetNewLogicalNodeId(),
                                                                    std::static_pointer_cast<BaseTableRef>(table_ref_ptr_),
                                                                    limit_expression_,
                                                                    offset_expression_,
                                                                    order_by_expressions_,
                                                                    order_by_types_,
                                                                    total_hits_count_flag_);
                top->set_left_node(root);
                root = top;
            }
        } else if (limit_expression_.get() != nullptr) {
            auto limit = MakeShared<LogicalLimit>(bind_context->GetNewLogicalNodeId(),
                                                  std::static_pointer_cast<BaseTableRef>(table_ref_ptr_),
                                                  limit_expression_,
                                                  offset_expression_,
                                                  total_hits_count_flag_);
            limit->set_left_node(root);
            root = limit;
        }

        auto project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(),
                                                  projection_expressions_,
                                                  projection_index_,
                                                  Map<SizeT, SharedPtr<HighlightInfo>>());
        project->set_left_node(root);
        root = project;

        if (!pruned_expression_.empty()) {
            String error_message = "Projection method changed!";
            UnrecoverableError(error_message);
            auto pruned_project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(),
                                                             pruned_expression_,
                                                             result_index_,
                                                             Map<SizeT, SharedPtr<HighlightInfo>>());
            pruned_project->set_left_node(root);
            root = pruned_project;
        }

        return root;
    } else {
        SharedPtr<LogicalNode> root = nullptr;
        const SizeT num_children = search_expr_->match_exprs_.size();
        if (num_children <= 0) {
            String error_message = "SEARCH shall have at least one MATCH TEXT or MATCH VECTOR or MATCH TENSOR or MATCH SPARSE expression";
            UnrecoverableError(error_message);
        }
        if (table_ref_ptr_->type() != TableRefType::kTable) {
            String error_message = "Not base table reference";
            UnrecoverableError(error_message);
        }
        auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref_ptr_);
        // FIXME: need check if there is subquery inside the where conditions
        auto default_filter_expr = ComposeExpressionWithDelimiter(where_conditions_, ConjunctionType::kAnd);
        if (default_filter_expr && search_expr_->have_filter_in_subsearch_) {
            RecoverableError(Status::SyntaxError("Cannot have filter in subsearch and where clause at the same time."));
        }

        NewTxn *new_txn_ptr = query_context->GetNewTxn();

        SharedPtr<CommonQueryFilter> default_common_query_filter;
        default_common_query_filter = MakeShared<CommonQueryFilter>(default_filter_expr, base_table_ref, new_txn_ptr);

        Vector<SharedPtr<LogicalNode>> match_knn_nodes;
        match_knn_nodes.reserve(num_children);
        for (auto &match_expr : search_expr_->match_exprs_) {
            auto filter_expr = default_filter_expr;
            auto common_query_filter = default_common_query_filter;
            switch (match_expr->type()) {
                case ExpressionType::kMatch: {
                    auto match_text_expr = std::dynamic_pointer_cast<MatchExpression>(match_expr);
                    if (match_text_expr->optional_filter_) {
                        filter_expr = match_text_expr->optional_filter_;
                        common_query_filter = MakeShared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    SharedPtr<LogicalMatch> match_node =
                        MakeShared<LogicalMatch>(bind_context->GetNewLogicalNodeId(), base_table_ref, std::move(match_text_expr));
                    match_node->filter_expression_ = std::move(filter_expr);
                    match_node->common_query_filter_ = std::move(common_query_filter);

                    Status status = new_txn_ptr->GetFullTextIndexReader(*base_table_ref->table_info_->db_name_,
                                                                        *base_table_ref->table_info_->table_name_,
                                                                        match_node->index_reader_);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("Get full text index reader error: {}", status.message()));
                    }

                    Map<String, String> column2analyzer = match_node->index_reader_->GetColumn2Analyzer(match_node->match_expr_->index_names_);
                    SearchOptions search_ops(match_node->match_expr_->options_text_);

                    // option: begin_threshold
                    const String &threshold = search_ops.options_["begin_threshold"];
                    match_node->begin_threshold_ = strtof(threshold.c_str(), nullptr);

                    // option: default field
                    auto iter = search_ops.options_.find("default_field");
                    String default_field;
                    if (iter != search_ops.options_.end()) {
                        default_field = iter->second;
                    }

                    // option: block max
                    iter = search_ops.options_.find("block_max");
                    if (iter == search_ops.options_.end() || iter->second == "auto") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kAuto;
                    } else if (iter->second == "true" || iter->second == "bmw") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kBMW;
                    } else if (iter->second == "batch") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kBatch;
                    } else if (iter->second == "false") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kNaive;
                    } else if (iter->second == "compare") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kCompare;
                    } else {
                        RecoverableError(Status::SyntaxError("block_max option must be empty, auto, bmw, true, batch, false, or compare"));
                    }

                    // option: top n
                    iter = search_ops.options_.find("topn");
                    if (iter != search_ops.options_.end()) {
                        i32 top_n_option = std::strtol(iter->second.c_str(), nullptr, 0);
                        if (top_n_option <= 0) {
                            Status status = Status::SyntaxError("top n must be a positive integer");
                            RecoverableError(status);
                        }
                        match_node->top_n_ = top_n_option;
                    } else {
                        match_node->top_n_ = DEFAULT_MATCH_TEXT_OPTION_TOP_N;
                    }

                    auto query_operator_option = FulltextQueryOperatorOption::kInfinitySyntax;
                    // option: operator
                    if (iter = search_ops.options_.find("operator"); iter != search_ops.options_.end()) {
                        ToLower(iter->second);
                        if (iter->second == "and") {
                            query_operator_option = FulltextQueryOperatorOption::kAnd;
                        } else if (iter->second == "or") {
                            query_operator_option = FulltextQueryOperatorOption::kOr;
                        } else {
                            RecoverableError(Status::SyntaxError(R"(operator option must be "and" or "or".)"));
                        }
                    }

                    // option: minimum_should_match
                    if (iter = search_ops.options_.find("minimum_should_match"); iter != search_ops.options_.end()) {
                        match_node->minimum_should_match_option_ = ParseMinimumShouldMatchOption(iter->second);
                    }

                    // option: rank_features
                    if (iter = search_ops.options_.find("rank_features"); iter != search_ops.options_.end()) {
                        match_node->rank_features_option_ = ParseRankFeaturesOption(iter->second);
                    }

                    // option: threshold
                    if (iter = search_ops.options_.find("threshold"); iter != search_ops.options_.end()) {
                        match_node->score_threshold_ = DataType::StringToValue<FloatT>(iter->second);
                    }

                    // option: similarity
                    if (iter = search_ops.options_.find("similarity"); iter != search_ops.options_.end()) {
                        String ft_sim = iter->second;
                        ToLower(ft_sim);
                        if (ft_sim == "bm25") {
                            match_node->ft_similarity_ = FulltextSimilarity::kBM25;
                        } else if (ft_sim == "boolean") {
                            match_node->ft_similarity_ = FulltextSimilarity::kBoolean;
                        } else {
                            RecoverableError(Status::SyntaxError(R"(similarity option must be "BM25" or "boolean".)"));
                        }
                    }
                    // option: bm25_params
                    if (iter = search_ops.options_.find("bm25_param_k1"); iter != search_ops.options_.end()) {
                        const auto k1_v = DataType::StringToValue<FloatT>(iter->second);
                        if (k1_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_k1 must be a non-negative float. default value: 1.2"));
                        }
                        match_node->bm25_params_.k1 = k1_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_b"); iter != search_ops.options_.end()) {
                        const auto b_v = DataType::StringToValue<FloatT>(iter->second);
                        if (b_v < 0.0f || b_v > 1.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_b must be in the range [0.0f, 1.0f]. default value: 0.75"));
                        }
                        match_node->bm25_params_.b = b_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_delta"); iter != search_ops.options_.end()) {
                        const auto delta_v = DataType::StringToValue<FloatT>(iter->second);
                        if (delta_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_delta must be a non-negative float. default value: 0.0"));
                        }
                        match_node->bm25_params_.delta_term = delta_v;
                        match_node->bm25_params_.delta_phrase = delta_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_delta_term"); iter != search_ops.options_.end()) {
                        const auto delta_term_v = DataType::StringToValue<FloatT>(iter->second);
                        if (delta_term_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_delta_term must be a non-negative float. default value: 0.0"));
                        }
                        match_node->bm25_params_.delta_term = delta_term_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_delta_phrase"); iter != search_ops.options_.end()) {
                        const auto delta_phrase_v = DataType::StringToValue<FloatT>(iter->second);
                        if (delta_phrase_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_delta_phrase must be a non-negative float. default value: 0.0"));
                        }
                        match_node->bm25_params_.delta_phrase = delta_phrase_v;
                    }

                    SearchDriver search_driver(column2analyzer, default_field, query_operator_option);
                    UniquePtr<QueryNode> query_tree =
                        search_driver.ParseSingleWithFields(match_node->match_expr_->fields_, match_node->match_expr_->matching_text_);
                    if (query_tree.get() == nullptr) {
                        Status status = Status::ParseMatchExprFailed(match_node->match_expr_->fields_, match_node->match_expr_->matching_text_);
                        RecoverableError(status);
                    }

                    // Initialize highlight info
                    if (!highlight_columns_.empty()) {
                        Vector<String> columns, terms;
                        query_tree->GetQueryColumnsTerms(columns, terms);

                        // Deduplicate columns
                        std::sort(columns.begin(), columns.end());
                        columns.erase(std::unique(columns.begin(), columns.end()), columns.end());

                        for (auto &column_name : columns) {
                            for (auto &[highlight_column_id, highlight_info] : highlight_columns_) {
                                if (column_name == projection_expressions_[highlight_column_id]->Name()) {
                                    highlight_info->query_terms_.insert(highlight_info->query_terms_.end(), terms.begin(), terms.end());
                                    const auto &it = column2analyzer.find(column_name);
                                    if (it == column2analyzer.end()) {
                                        highlight_info->analyzer_ = "standard";
                                    } else {
                                        highlight_info->analyzer_ = it->second;
                                    }
                                }
                            }
                        }
                    }

                    match_node->query_tree_ = std::move(query_tree);
                    match_knn_nodes.push_back(std::move(match_node));
                    break;
                }
                case ExpressionType::kKnn: {
                    auto match_dense_expr = std::dynamic_pointer_cast<KnnExpression>(match_expr);
                    if (match_dense_expr->optional_filter_) {
                        filter_expr = match_dense_expr->optional_filter_;
                        common_query_filter = MakeShared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    auto knn_scan = MakeShared<LogicalKnnScan>(bind_context->GetNewLogicalNodeId(),
                                                               base_table_ref,
                                                               std::move(match_dense_expr),
                                                               bind_context->knn_table_index_);
                    knn_scan->filter_expression_ = std::move(filter_expr);
                    knn_scan->common_query_filter_ = std::move(common_query_filter);
                    match_knn_nodes.push_back(std::move(knn_scan));
                    break;
                }
                case ExpressionType::kMatchTensor: {
                    auto match_tensor_expr = std::dynamic_pointer_cast<MatchTensorExpression>(match_expr);
                    if (match_tensor_expr->optional_filter_) {
                        filter_expr = match_tensor_expr->optional_filter_;
                        common_query_filter = MakeShared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    auto match_tensor_node =
                        MakeShared<LogicalMatchTensorScan>(bind_context->GetNewLogicalNodeId(), base_table_ref, std::move(match_tensor_expr));
                    match_tensor_node->filter_expression_ = std::move(filter_expr);
                    match_tensor_node->common_query_filter_ = std::move(common_query_filter);
                    match_tensor_node->InitExtraOptions();
                    match_knn_nodes.push_back(std::move(match_tensor_node));
                    break;
                }
                case ExpressionType::kMatchSparse: {
                    auto match_sparse_expr = std::dynamic_pointer_cast<MatchSparseExpression>(match_expr);
                    if (match_sparse_expr->optional_filter_) {
                        filter_expr = match_sparse_expr->optional_filter_;
                        common_query_filter = MakeShared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    auto match_sparse_node =
                        MakeShared<LogicalMatchSparseScan>(bind_context->GetNewLogicalNodeId(), base_table_ref, std::move(match_sparse_expr));
                    match_sparse_node->filter_expression_ = std::move(filter_expr);
                    match_sparse_node->common_query_filter_ = std::move(common_query_filter);
                    match_knn_nodes.push_back(std::move(match_sparse_node));
                    break;
                }
                default: {
                    UnrecoverableError(fmt::format("Unsupported match expression: {}.", match_expr->ToString()));
                }
            }
        }
        bind_context->GenerateTableIndex();
        if (!(search_expr_->fusion_exprs_.empty())) {
            auto firstfusionNode = MakeShared<LogicalFusion>(bind_context->GetNewLogicalNodeId(), base_table_ref, search_expr_->fusion_exprs_[0]);
            firstfusionNode->set_left_node(match_knn_nodes[0]);
            if (match_knn_nodes.size() > 1) {
                firstfusionNode->set_right_node(match_knn_nodes[1]);
                if (match_knn_nodes.size() > 2) {
                    for (SizeT i = 2; i < match_knn_nodes.size(); i++) {
                        firstfusionNode->other_children_.push_back(std::move(match_knn_nodes[i]));
                    }
                }
            }
            root = std::move(firstfusionNode);
            // extra fusion nodes
            for (u32 i = 1; i < search_expr_->fusion_exprs_.size(); ++i) {
                auto extrafusionNode = MakeShared<LogicalFusion>(bind_context->GetNewLogicalNodeId(), base_table_ref, search_expr_->fusion_exprs_[i]);
                extrafusionNode->set_left_node(root);
                root = std::move(extrafusionNode);
            }
        } else {
            root = std::move(match_knn_nodes[0]);
        }

        if (limit_expression_.get() != nullptr) {
            auto limit = MakeShared<LogicalLimit>(bind_context->GetNewLogicalNodeId(),
                                                  base_table_ref,
                                                  limit_expression_,
                                                  offset_expression_,
                                                  total_hits_count_flag_);
            limit->set_left_node(root);
            root = limit;
        }

        // Finalize highlight info
        if (!highlight_columns_.empty()) {
            for (auto &[highlight_column_id, highlight_info] : highlight_columns_) {
                // Deduplicate terms
                std::sort(highlight_info->query_terms_.begin(), highlight_info->query_terms_.end());
                highlight_info->query_terms_.erase(std::unique(highlight_info->query_terms_.begin(), highlight_info->query_terms_.end()),
                                                   highlight_info->query_terms_.end());
            }
        }

        auto project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(),
                                                  projection_expressions_,
                                                  projection_index_,
                                                  std::move(highlight_columns_));
        project->set_left_node(root);
        root = std::move(project);

        return root;
    }
}

SharedPtr<LogicalKnnScan> BoundSelectStatement::BuildInitialKnnScan(SharedPtr<TableRef> &table_ref,
                                                                    SharedPtr<KnnExpression> knn_expr,
                                                                    QueryContext *query_context,
                                                                    const SharedPtr<BindContext> &bind_context) {
    if (table_ref.get() == nullptr) {
        String error_message = "Attempt to do KNN scan without table";
        UnrecoverableError(error_message);
    }
    switch (table_ref->type_) {
        case TableRefType::kCrossProduct: {
            String error_message = "KNN is not supported on CROSS PRODUCT relation, now.";
            UnrecoverableError(error_message);
            break;
        }
        case TableRefType::kJoin: {
            String error_message = "KNN is not supported on JOIN relation, now.";
            UnrecoverableError(error_message);
        }
        case TableRefType::kTable: {
            auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref);
            // Change function table to knn table scan function
            SharedPtr<LogicalKnnScan> knn_scan_node = MakeShared<LogicalKnnScan>(bind_context->GetNewLogicalNodeId(),
                                                                                 std::move(base_table_ref),
                                                                                 std::move(knn_expr),
                                                                                 bind_context->knn_table_index_);
            return knn_scan_node;
        }
        case TableRefType::kSubquery: {
            String error_message = "KNN is not supported on a SUBQUERY, now.";
            UnrecoverableError(error_message);
            break;
        }
        default: {
            String error_message = "Unexpected table type";
            UnrecoverableError(error_message);
        }
    }

    return nullptr;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildFrom(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context) {
    if (table_ref) {
        switch (table_ref->type_) {
            case TableRefType::kTable: {
                return BuildBaseTable(table_ref, query_context, bind_context);
            }
            case TableRefType::kSubquery: {
                return BuildSubqueryTable(table_ref, query_context, bind_context);
            }
            case TableRefType::kCrossProduct: {
                return BuildCrossProductTable(table_ref, query_context, bind_context);
            }
            case TableRefType::kJoin: {
                return BuildJoinTable(table_ref, query_context, bind_context);
            }
            case TableRefType::kDummy: {
                return BuildDummyTable(table_ref, query_context, bind_context);
            }
            default: {
                String error_message = "Unknown table reference type.";
                UnrecoverableError(error_message);
            }
        }
    } else {
        // No FROM-Clause, only select constant expression.
    }

    return nullptr;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildBaseTable(SharedPtr<TableRef> &table_ref, QueryContext *, const SharedPtr<BindContext> &bind_context) {
    // SharedPtr<BaseTableRef> base_table_ref
    auto base_table_ref = static_pointer_cast<BaseTableRef>(table_ref);

    SharedPtr<LogicalTableScan> table_scan_node = MakeShared<LogicalTableScan>(bind_context->GetNewLogicalNodeId(), base_table_ref);
    return table_scan_node;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildSubqueryTable(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &) {
    // SharedPtr<SubqueryTableRef> subquery_table_ref
    auto subquery_table_ref = static_pointer_cast<SubqueryTableRef>(table_ref);
    SharedPtr<LogicalNode> subquery = subquery_table_ref->subquery_node_->BuildPlan(query_context);
    return subquery;
}

SharedPtr<LogicalNode> BoundSelectStatement::BuildCrossProductTable(SharedPtr<TableRef> &table_ref,
                                                                    QueryContext *query_context,
                                                                    const SharedPtr<BindContext> &bind_context) {
    // SharedPtr<CrossProductTableRef> cross_product_table_ref
    auto cross_product_table_ref = static_pointer_cast<CrossProductTableRef>(table_ref);

    auto left_node = BuildFrom(cross_product_table_ref->left_table_ref_, query_context, bind_context);
    auto right_node = BuildFrom(cross_product_table_ref->right_table_ref_, query_context, bind_context);

    // TODO: Merge bind context ?
    u64 logical_node_id = bind_context->GetNewLogicalNodeId();
    String alias(fmt::format("cross_product{}", logical_node_id));
    SharedPtr<LogicalCrossProduct> logical_cross_product_node = MakeShared<LogicalCrossProduct>(logical_node_id, alias, left_node, right_node);
    return logical_cross_product_node;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildJoinTable(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context) {
    // SharedPtr<JoinTableRef> join_table_ref
    auto join_table_ref = static_pointer_cast<JoinTableRef>(table_ref);

    auto left_node = BuildFrom(join_table_ref->left_table_ref_, query_context, bind_context);
    auto right_node = BuildFrom(join_table_ref->right_table_ref_, query_context, bind_context);

    // TODO: Merge bind context ?
    u64 logical_node_id = bind_context->GetNewLogicalNodeId();
    String alias(fmt::format("join{}", logical_node_id));
    SharedPtr<LogicalJoin> logical_join_node =
        MakeShared<LogicalJoin>(logical_node_id, join_table_ref->join_type_, alias, join_table_ref->on_conditions_, left_node, right_node);
    return logical_join_node;
}

SharedPtr<LogicalNode> BoundSelectStatement::BuildDummyTable(SharedPtr<TableRef> &, QueryContext *, const SharedPtr<BindContext> &bind_context) {
    u64 logical_node_id = bind_context->GetNewLogicalNodeId();
    String alias(fmt::format("DummyTable{}", logical_node_id));
    SharedPtr<LogicalDummyScan> dummy_scan_node = MakeShared<LogicalDummyScan>(logical_node_id, alias, bind_context->GenerateTableIndex());
    return dummy_scan_node;
}

SharedPtr<LogicalNode> BoundSelectStatement::BuildFilter(SharedPtr<LogicalNode> &root,
                                                         Vector<SharedPtr<BaseExpression>> &conditions,
                                                         QueryContext *query_context,
                                                         const SharedPtr<BindContext> &bind_context) {
    for (auto &cond : conditions) {
        // 1. Go through all the expression to find subquery
        //        VisitExpression(cond,
        //                        [&](SharedPtr<BaseExpression> &expr) {
        //                            SubqueryUnnest::UnnestSubqueries(expr, root, bind_context);
        //                        });
        BuildSubquery(root, cond, query_context, bind_context);
    }

    // SharedPtr<BaseExpression> filter_expr
    auto filter_expr = ComposeExpressionWithDelimiter(conditions, ConjunctionType::kAnd);

    // SharedPtr<LogicalFilter> filter
    auto filter = MakeShared<LogicalFilter>(bind_context->GetNewLogicalNodeId(), filter_expr);

    return filter;
}

SharedPtr<LogicalNode> BoundSelectStatement::BuildUnnest(SharedPtr<LogicalNode> &root,
                                                         Vector<SharedPtr<BaseExpression>> &expressions,
                                                         QueryContext *query_context,
                                                         const SharedPtr<BindContext> &bind_context) {
    // SharedPtr<LogicalUnnest> unnest
    expressions = {bind_context->unnest_exprs_};
    SizeT unnest_idx = bind_context->unnest_table_index_;
    auto unnest = MakeShared<LogicalUnnest>(bind_context->GetNewLogicalNodeId(), expressions, unnest_idx);
    return unnest;
}

void BoundSelectStatement::BuildSubquery(SharedPtr<LogicalNode> &root,
                                         SharedPtr<BaseExpression> &condition,
                                         QueryContext *query_context,
                                         const SharedPtr<BindContext> &bind_context) {
    if (condition.get() == nullptr) {
        return;
    }

    VisitExpression(condition, [&](SharedPtr<BaseExpression> &expr) { BuildSubquery(root, expr, query_context, bind_context); });

    if (condition->type() == ExpressionType::kSubQuery) {
        if (building_subquery_) {
            // nested subquery
            String error_message = "Nested subquery detected";
            UnrecoverableError(error_message);
        }
        condition = UnnestSubquery(root, condition, query_context, bind_context);
    }
}

SharedPtr<BaseExpression> BoundSelectStatement::UnnestSubquery(SharedPtr<LogicalNode> &root,
                                                               SharedPtr<BaseExpression> &condition,
                                                               QueryContext *query_context,
                                                               const SharedPtr<BindContext> &) {
    building_subquery_ = true;
    SubqueryExpression *subquery_expr_ptr = (SubqueryExpression *)condition.get();
    SharedPtr<LogicalNode> subquery_plan = subquery_expr_ptr->bound_select_statement_ptr_->BuildPlan(query_context);
    SharedPtr<BaseExpression> return_expr = nullptr;
    if (subquery_expr_ptr->bound_select_statement_ptr_->bind_context_->HasCorrelatedColumn()) {
        // If correlated subquery
        return_expr = SubqueryUnnest::UnnestCorrelated(subquery_expr_ptr,
                                                       root,
                                                       subquery_plan,
                                                       query_context,
                                                       subquery_expr_ptr->bound_select_statement_ptr_->bind_context_);
    } else {
        // If uncorrelated subquery
        return_expr = SubqueryUnnest::UnnestUncorrelated(subquery_expr_ptr,
                                                         root,
                                                         subquery_plan,
                                                         query_context,
                                                         subquery_expr_ptr->bound_select_statement_ptr_->bind_context_);
    }
    building_subquery_ = false;
    return return_expr;
}

} // namespace infinity
