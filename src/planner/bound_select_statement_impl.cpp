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

module infinity_core:bound_select_statement.impl;

import :bound_select_statement;
import :logical_node;
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
import :aggregate_expression;
import :aggregate_function;
import :aggregate_function_set;
import :subquery_table_ref;
import :cross_product_table_ref;
import :join_table_ref;
import :knn_expression;
import :match_expression;
import :match_tensor_expression;
import :match_sparse_expression;
import :common_query_filter;
import :search_driver;
import :query_node;
import :doc_iterator;
import :status;
import :default_values;
import :parse_fulltext_options;
import :highlighter;
import :new_txn;
import :utility;

import std.compat;
import third_party;

import table_reference;
import internal_types;
import data_type;
import search_options;

namespace infinity {

std::shared_ptr<LogicalNode> BoundSelectStatement::BuildPlan(QueryContext *query_context) {
    const std::shared_ptr<BindContext> &bind_context = this->bind_context_;
    std::shared_ptr<LogicalNode> root = nullptr;
    if (search_expr_.get() == nullptr) {
        root = BuildFrom(table_ref_ptr_, query_context, bind_context);

        if (!unnest_expressions_.empty()) {
            std::shared_ptr<LogicalNode> unnest = BuildUnnest(root, unnest_expressions_, query_context, bind_context);
            unnest->set_left_node(root);
            root = unnest;
        }

        if (!where_conditions_.empty()) {
            std::shared_ptr<LogicalNode> filter = BuildFilter(root, where_conditions_, query_context, bind_context);
            filter->set_left_node(root);
            root = filter;
        }
    } else {
        const size_t num_children = search_expr_->match_exprs_.size();
        if (num_children <= 0) {
            UnrecoverableError("SEARCH shall have at least one MATCH TEXT or MATCH VECTOR or MATCH TENSOR or MATCH SPARSE expression");
        }
        if (table_ref_ptr_->type() != TableRefType::kTable) {
            UnrecoverableError("Not base table reference");
        }
        auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref_ptr_);
        // FIXME: need check if there is subquery inside the where conditions
        auto default_filter_expr = ComposeExpressionWithDelimiter(where_conditions_, ConjunctionType::kAnd);
        if (default_filter_expr && search_expr_->have_filter_in_subsearch_) {
            RecoverableError(Status::SyntaxError("Cannot have filter in subsearch and where clause at the same time."));
        }

        NewTxn *new_txn_ptr = query_context->GetNewTxn();

        std::shared_ptr<CommonQueryFilter> default_common_query_filter;
        default_common_query_filter = std::make_shared<CommonQueryFilter>(default_filter_expr, base_table_ref, new_txn_ptr);

        std::vector<std::shared_ptr<LogicalNode>> match_knn_nodes;
        match_knn_nodes.reserve(num_children);
        for (auto &match_expr : search_expr_->match_exprs_) {
            auto filter_expr = default_filter_expr;
            auto common_query_filter = default_common_query_filter;
            switch (match_expr->type()) {
                case ExpressionType::kMatch: {
                    auto match_text_expr = std::dynamic_pointer_cast<MatchExpression>(match_expr);
                    if (match_text_expr->optional_filter_) {
                        filter_expr = match_text_expr->optional_filter_;
                        common_query_filter = std::make_shared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    std::shared_ptr<LogicalMatch> match_node =
                        std::make_shared<LogicalMatch>(bind_context->GetNewLogicalNodeId(), base_table_ref, std::move(match_text_expr));
                    match_node->filter_expression_ = std::move(filter_expr);
                    match_node->common_query_filter_ = std::move(common_query_filter);

                    Status status = new_txn_ptr->GetFullTextIndexReader(*base_table_ref->table_info_->db_name_,
                                                                        *base_table_ref->table_info_->table_name_,
                                                                        match_node->index_reader_);
                    if (!status.ok()) {
                        UnrecoverableError(fmt::format("Get full text index reader error: {}", status.message()));
                    }

                    std::map<std::string, std::map<std::string, std::string>> column2analyzer = match_node->index_reader_->GetColumn2Analyzer();
                    SearchOptions search_ops(match_node->match_expr_->options_text_);

                    // option: begin_threshold
                    const std::string &threshold = search_ops.options_["begin_threshold"];
                    match_node->begin_threshold_ = strtof(threshold.c_str(), nullptr);

                    // option: default field
                    auto iter = search_ops.options_.find("default_field");
                    std::string default_field;
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
                        std::string ft_sim = iter->second;
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

                    SearchDriver search_driver(std::move(column2analyzer), default_field, query_operator_option);
                    std::unique_ptr<QueryNode> query_tree =
                        search_driver.ParseSingleWithFields(match_node->match_expr_->fields_, match_node->match_expr_->matching_text_);
                    if (query_tree.get() == nullptr) {
                        Status status = Status::ParseMatchExprFailed(match_node->match_expr_->fields_, match_node->match_expr_->matching_text_);
                        RecoverableError(status);
                    }

                    // Initialize highlight info
                    if (!highlight_columns_.empty()) {
                        std::vector<std::string> columns, terms;
                        query_tree->GetQueryColumnsTerms(columns, terms);

                        // Deduplicate columns
                        std::ranges::sort(columns);
                        columns.erase(std::unique(columns.begin(), columns.end()), columns.end());

                        for (auto &column_name : columns) {
                            for (auto &[highlight_column_id, highlight_info] : highlight_columns_) {
                                if (column_name == projection_expressions_[highlight_column_id]->Name()) {
                                    highlight_info->matching_text_ = match_node->match_expr_->matching_text_;
                                    highlight_info->analyzer_ = "rag-coarse";
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
                        common_query_filter = std::make_shared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    auto knn_scan = std::make_shared<LogicalKnnScan>(bind_context->GetNewLogicalNodeId(),
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
                        common_query_filter = std::make_shared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    auto match_tensor_node =
                        std::make_shared<LogicalMatchTensorScan>(bind_context->GetNewLogicalNodeId(), base_table_ref, std::move(match_tensor_expr));
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
                        common_query_filter = std::make_shared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetNewTxn());
                    }
                    auto match_sparse_node =
                        std::make_shared<LogicalMatchSparseScan>(bind_context->GetNewLogicalNodeId(), base_table_ref, std::move(match_sparse_expr));
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
            auto firstfusionNode =
                std::make_shared<LogicalFusion>(bind_context->GetNewLogicalNodeId(), base_table_ref, search_expr_->fusion_exprs_[0]);
            firstfusionNode->set_left_node(match_knn_nodes[0]);
            if (match_knn_nodes.size() > 1) {
                firstfusionNode->set_right_node(match_knn_nodes[1]);
                if (match_knn_nodes.size() > 2) {
                    for (size_t i = 2; i < match_knn_nodes.size(); i++) {
                        firstfusionNode->other_children_.push_back(std::move(match_knn_nodes[i]));
                    }
                }
            }
            root = std::move(firstfusionNode);
            // extra fusion nodes
            for (u32 i = 1; i < search_expr_->fusion_exprs_.size(); ++i) {
                auto extrafusionNode =
                    std::make_shared<LogicalFusion>(bind_context->GetNewLogicalNodeId(), base_table_ref, search_expr_->fusion_exprs_[i]);
                extrafusionNode->set_left_node(root);
                root = std::move(extrafusionNode);
            }
        } else {
            root = std::move(match_knn_nodes[0]);
        }
    }

    if (!group_by_expressions_.empty() || !aggregate_expressions_.empty()) {
        // Build logical aggregate
        auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref_ptr_);

        // Check for aggregates with DISTINCT
        bool has_distinct_agg = false;
        bool has_non_distinct_agg = false;
        for (const auto &agg_expr : aggregate_expressions_) {
            auto *ae = static_cast<AggregateExpression *>(agg_expr.get());
            if (ae->distinct()) {
                has_distinct_agg = true;
            } else {
                has_non_distinct_agg = true;
            }

            if (has_distinct_agg && has_non_distinct_agg) {
                Status status = Status::NotSupport("Mixing DISTINCT and non-DISTINCT aggregates is not yet supported");
                RecoverableError(status);
            }
        }

        // Transform aggregates with DISTINCT to two-level aggregation
        if (has_distinct_agg) {
            std::set<std::vector<std::string>> distinct_arg_sets;

            for (const auto &agg_expr : aggregate_expressions_) {
                auto *ae = static_cast<AggregateExpression *>(agg_expr.get());
                const auto &args = ae->arguments();
                std::vector<std::string> arg_strs;
                for (const auto &arg : args) {
                    arg_strs.push_back(arg->ToString());
                }
                distinct_arg_sets.insert(arg_strs);

                if (args.size() > 1) {
                    std::string func_name = ae->aggregate_function_.name();
                    // Only COUNT(DISTINCT) supports multiple arguments.
                    if (func_name != "COUNT") {
                        Status status = Status::NotSupport(fmt::format("{}(DISTINCT) with multiple arguments is not supported.", func_name));
                        RecoverableError(status);
                    }
                }
            }

            // Reject multiple distinct aggregates on different columns (e.g. sum(DISTINCT c1), sum(DISTINCT c2))
            // as it requires separate aggregation paths
            if (distinct_arg_sets.size() > 1) {
                Status status = Status::NotSupport("Multiple DISTINCT aggregates on different columns are not supported.");
                RecoverableError(status);
            }

            // Step 1: Collect DISTINCT arguments and original GROUP BY columns as GROUP BY keys
            std::vector<std::shared_ptr<BaseExpression>> first_level_groups;
            std::vector<AggregateExpression *> distinct_aggs_to_process;

            auto *first_distinct_agg = static_cast<AggregateExpression *>(aggregate_expressions_[0].get());
            std::set<BaseExpression *> distinct_arg_ptrs;
            for (const auto &arg : first_distinct_agg->arguments()) {
                first_level_groups.push_back(arg);
                distinct_arg_ptrs.insert(arg.get());
            }
            for (const auto &agg_expr : aggregate_expressions_) {
                distinct_aggs_to_process.push_back(static_cast<AggregateExpression *>(agg_expr.get()));
            }

            for (const auto &group_by_expr : group_by_expressions_) {
                bool already_in_distinct = false;
                for (const auto *arg_ptr : distinct_arg_ptrs) {
                    if (arg_ptr->ToString() == group_by_expr->ToString()) {
                        already_in_distinct = true;
                        break;
                    }
                }
                if (!already_in_distinct) {
                    first_level_groups.push_back(group_by_expr);
                }
            }

            std::vector<std::vector<size_t>> agg_to_groupby_indices;
            for (const auto *ae : distinct_aggs_to_process) {
                const auto &args = ae->arguments();
                std::vector<size_t> groupby_col_indices;

                for (const auto &arg : args) {
                    for (size_t i = 0; i < first_level_groups.size(); ++i) {
                        if (first_level_groups[i]->ToString() == arg->ToString()) {
                            groupby_col_indices.push_back(i);
                            break;
                        }
                    }
                }
                agg_to_groupby_indices.push_back(groupby_col_indices);
            }

            // Step 2: Build first aggregate (GROUP BY distinct columns, no aggregates)
            // First aggregate removes duplicates across tasklets, returns one row per distinct value
            u64 first_agg_output_index = bind_context->GenerateTableIndex();
            auto first_agg = std::make_shared<LogicalAggregate>(bind_context->GetNewLogicalNodeId(),
                                                                base_table_ref,
                                                                first_level_groups,
                                                                groupby_index_,
                                                                std::vector<std::shared_ptr<BaseExpression>>{},
                                                                first_agg_output_index); // UNIQUE index for first aggregate output
            first_agg->set_left_node(root);
            root = first_agg;

            auto first_agg_output_types = first_agg->GetOutputTypes();
            auto first_agg_output_names = first_agg->GetOutputNames();
            std::string first_agg_table_name = "__distinct_agg_result_" + std::to_string(first_agg_output_index);
            bind_context->AddSubqueryBinding(first_agg_table_name, first_agg_output_index, first_agg_output_types, first_agg_output_names);

            // Step 3: Build aggregates on grouped results
            // For COUNT(DISTINCT): Use COUNT(*) to count rows after GROUP BY
            // For other aggregates (SUM, AVG, etc.): Use the only DISTINCT argument
            std::vector<std::shared_ptr<BaseExpression>> second_level_aggregates;
            NewCatalog *catalog = query_context->storage()->new_catalog();

            for (size_t agg_idx = 0; auto *orig_agg : distinct_aggs_to_process) {
                std::vector<std::shared_ptr<BaseExpression>> agg_column_exprs;
                const std::vector<size_t> &groupby_col_indices = agg_to_groupby_indices[agg_idx];
                size_t col_idx = groupby_col_indices[0];
                auto group_by_col_expr = ColumnExpression::Make(*(*first_agg_output_types)[col_idx],
                                                                first_agg_table_name,
                                                                groupby_index_,
                                                                (*first_agg_output_names)[col_idx],
                                                                col_idx,
                                                                0);
                group_by_col_expr->source_position_ = SourcePosition(bind_context->binding_context_id_, ExprSourceType::kBinding);
                group_by_col_expr->source_position_.binding_name_ = first_agg_table_name;

                std::string func_name = orig_agg->aggregate_function_.name();
                std::shared_ptr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, func_name);
                auto aggregate_function_set_ptr = static_pointer_cast<AggregateFunctionSet>(function_set_ptr);
                AggregateFunction aggregate_function = aggregate_function_set_ptr->GetMostMatchFunction(group_by_col_expr);

                std::vector<std::shared_ptr<BaseExpression>> final_agg_args;
                final_agg_args.push_back(group_by_col_expr);

                auto final_agg = std::make_shared<AggregateExpression>(aggregate_function, final_agg_args);
                final_agg->SetCountStar(func_name == "COUNT");

                second_level_aggregates.push_back(final_agg);
                ++agg_idx;
            }

            u64 second_agg_output_index = bind_context->GenerateTableIndex();
            u64 second_agg_groupby_index = bind_context->GenerateTableIndex();
            std::vector<std::shared_ptr<BaseExpression>> second_level_groups;
            if (!group_by_expressions_.empty()) {
                for (const auto &group_by_expr : group_by_expressions_) {
                    size_t col_idx = 0;
                    for (size_t i = 0; i < first_level_groups.size(); ++i) {
                        if (first_level_groups[i]->ToString() == group_by_expr->ToString()) {
                            col_idx = i;
                            break;
                        }
                    }
                    auto group_by_col_expr = ColumnExpression::Make(*(*first_agg_output_types)[col_idx],
                                                                    first_agg_table_name,
                                                                    groupby_index_,
                                                                    (*first_agg_output_names)[col_idx],
                                                                    col_idx,
                                                                    0);
                    group_by_col_expr->source_position_ = SourcePosition(bind_context->binding_context_id_, ExprSourceType::kBinding);
                    group_by_col_expr->source_position_.binding_name_ = first_agg_table_name;
                    second_level_groups.push_back(group_by_col_expr);
                }
            }

            auto second_agg = std::make_shared<LogicalAggregate>(bind_context->GetNewLogicalNodeId(),
                                                                 base_table_ref,
                                                                 second_level_groups,
                                                                 second_agg_groupby_index,
                                                                 second_level_aggregates,
                                                                 second_agg_output_index);

            auto second_agg_output_types = second_agg->GetOutputTypes();
            auto second_agg_output_names = second_agg->GetOutputNames();
            std::string second_agg_table_name = "__final_agg_result_" + std::to_string(second_agg_output_index);

            auto second_agg_bindings = second_agg->GetColumnBindings();
            bind_context->AddSubqueryBinding(second_agg_table_name, second_agg_output_index, second_agg_output_types, second_agg_output_names);

            second_agg->set_left_node(root);
            root = second_agg;
            bind_context->aggregate_table_index_ = second_agg_output_index;
            bind_context->aggregate_table_name_ = second_agg_table_name;

            for (size_t i = 0; i < projection_expressions_.size(); ++i) {
                auto &proj_expr = projection_expressions_[i];
                if (auto *col_expr = dynamic_cast<ColumnExpression *>(proj_expr.get())) {
                    if (col_expr->source_position_.source_type_ == ExprSourceType::kAggregate) {
                        size_t output_col_idx = col_expr->binding().column_idx;
                        auto new_col_expr = ColumnExpression::Make(col_expr->Type(),
                                                                   second_agg_table_name,
                                                                   second_agg_output_index,
                                                                   col_expr->column_name(),
                                                                   output_col_idx,
                                                                   col_expr->depth());
                        new_col_expr->source_position_ = col_expr->source_position_;
                        new_col_expr->alias_ = col_expr->alias_;
                        projection_expressions_[i] = new_col_expr;
                    }
                }
            }
            types_ptr_ = second_agg_output_types;
            names_ptr_ = second_agg_output_names;
        } else {
            auto aggregate = std::make_shared<LogicalAggregate>(bind_context->GetNewLogicalNodeId(),
                                                                base_table_ref,
                                                                group_by_expressions_,
                                                                groupby_index_,
                                                                aggregate_expressions_,
                                                                aggregate_index_);
            aggregate->set_left_node(root);
            root = aggregate;
        }
    }

    if (!having_expressions_.empty()) {
        // Build logical filter
        auto having_filter = BuildFilter(root, having_expressions_, query_context, bind_context);
        having_filter->set_left_node(root);
        root = having_filter;
    }

    // Helper lambdas for plan construction (improves readability and reusability)

    // Create column expressions that reference a specific table output
    auto create_column_exprs_from_table = [&](u64 table_index, const std::string &table_name) {
        std::vector<std::shared_ptr<BaseExpression>> exprs;
        exprs.reserve(projection_expressions_.size());
        for (size_t i = 0; i < projection_expressions_.size(); ++i) {
            const std::string &expr_name = names_ptr_->at(i);
            auto col_expr = ColumnExpression::Make(*types_ptr_->at(i), table_name, table_index, expr_name, i, 0);
            exprs.push_back(col_expr);
        }
        return exprs;
    };

    // Rewrite ORDER BY expressions to reference a different table (e.g., after DISTINCT)
    auto rewrite_order_by_expressions = [&](u64 table_index, const std::string &table_name) {
        std::vector<std::shared_ptr<BaseExpression>> new_exprs;
        new_exprs.reserve(order_by_expressions_.size());
        for (size_t i = 0; i < order_by_expressions_.size(); ++i) {
            const auto &order_expr = order_by_expressions_[i];

            // Find matching projection by name
            size_t found_idx = 0;
            for (size_t j = 0; j < projection_expressions_.size(); ++j) {
                if (projection_expressions_[j]->Name() == order_expr->Name()) {
                    found_idx = j;
                    break;
                }
            }

            const std::string &expr_name = names_ptr_->at(found_idx);
            auto col_expr = ColumnExpression::Make(*types_ptr_->at(found_idx), table_name, table_index, expr_name, found_idx, 0);
            new_exprs.push_back(col_expr);
        }
        return new_exprs;
    };

    // Add SORT or TOP (with LIMIT) node to the plan
    auto add_sort_or_top_node = [&](const std::vector<std::shared_ptr<BaseExpression>> &order_exprs) {
        if (limit_expression_.get() == nullptr) {
            auto sort = std::make_shared<LogicalSort>(bind_context->GetNewLogicalNodeId(), order_exprs, order_by_types_);
            sort->set_left_node(root);
            root = sort;
        } else {
            auto top = std::make_shared<LogicalTop>(bind_context->GetNewLogicalNodeId(),
                                                    std::static_pointer_cast<BaseTableRef>(table_ref_ptr_),
                                                    limit_expression_,
                                                    offset_expression_,
                                                    order_exprs,
                                                    order_by_types_,
                                                    total_hits_count_flag_);
            top->set_left_node(root);
            root = top;
        }
    };

    // Create a DISTINCT AGGREGATE node
    auto create_distinct_aggregate_node = [&](u64 table_index) {
        auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref_ptr_);
        auto aggregate = std::make_shared<LogicalAggregate>(bind_context->GetNewLogicalNodeId(),
                                                            base_table_ref,
                                                            projection_expressions_,
                                                            table_index,
                                                            std::vector<std::shared_ptr<BaseExpression>>(),
                                                            table_index);
        aggregate->set_left_node(root);
        return aggregate;
    };

    // Main plan construction logic
    bool has_order_by = !order_by_expressions_.empty();
    bool has_limit = limit_expression_.get() != nullptr;

    if (distinct_ && has_order_by) {
        // Case 1: DISTINCT with ORDER BY
        // Flow: ... → AGGREGATE(DISTINCT) → SORT/TOP → PROJECT
        u64 distinct_table_index = bind_context->GenerateTableIndex();
        std::string distinct_project_name = fmt::format("distinct_project_{}", distinct_table_index);

        // Step 1: Add AGGREGATE for DISTINCT
        root = create_distinct_aggregate_node(distinct_table_index);

        // Step 2: Add SORT/TOP with rewritten ORDER BY expressions
        auto new_order_by_exprs = rewrite_order_by_expressions(distinct_table_index, distinct_project_name);
        add_sort_or_top_node(new_order_by_exprs);

        // Step 3: Add PROJECT node
        auto output_project_exprs = create_column_exprs_from_table(distinct_table_index, distinct_project_name);
        auto project = std::make_shared<LogicalProject>(bind_context->GetNewLogicalNodeId(),
                                                        output_project_exprs,
                                                        projection_index_,
                                                        std::map<size_t, std::shared_ptr<HighlightInfo>>());
        project->set_left_node(root);
        root = project;

    } else {
        // Case 2: All other cases (no DISTINCT, or no ORDER BY)

        // Subcase 2a: Add ORDER BY/LIMIT nodes if present
        if (has_order_by) {
            if (order_by_expressions_.size() != order_by_types_.size()) {
                UnrecoverableError("Unknown error on order by expression");
            }
            add_sort_or_top_node(order_by_expressions_);
        } else if (has_limit) {
            auto limit = std::make_shared<LogicalLimit>(bind_context->GetNewLogicalNodeId(),
                                                        std::static_pointer_cast<BaseTableRef>(table_ref_ptr_),
                                                        limit_expression_,
                                                        offset_expression_,
                                                        total_hits_count_flag_);
            limit->set_left_node(root);
            root = limit;
        }

        // Subcase 2b: Add DISTINCT or final PROJECT node
        if (distinct_) {
            u64 distinct_table_index = bind_context->GenerateTableIndex();
            std::string distinct_project_name = fmt::format("distinct_project_{}", distinct_table_index);

            // Add AGGREGATE for DISTINCT
            root = create_distinct_aggregate_node(distinct_table_index);

            // Add PROJECT for DISTINCT output
            auto distinct_project_exprs = create_column_exprs_from_table(distinct_table_index, distinct_project_name);
            auto distinct_project = std::make_shared<LogicalProject>(bind_context->GetNewLogicalNodeId(),
                                                                     distinct_project_exprs,
                                                                     projection_index_,
                                                                     std::map<size_t, std::shared_ptr<HighlightInfo>>());
            distinct_project->set_left_node(root);
            root = distinct_project;
        } else {
            // Regular PROJECT (no DISTINCT)
            auto project = std::make_shared<LogicalProject>(bind_context->GetNewLogicalNodeId(),
                                                            projection_expressions_,
                                                            projection_index_,
                                                            std::move(highlight_columns_));
            project->set_left_node(root);
            root = project;
        }
    }

    if (!pruned_expression_.empty()) {
        UnrecoverableError("Projection method changed!");
        auto pruned_project = std::make_shared<LogicalProject>(bind_context->GetNewLogicalNodeId(),
                                                               pruned_expression_,
                                                               result_index_,
                                                               std::map<size_t, std::shared_ptr<HighlightInfo>>());
        pruned_project->set_left_node(root);
        root = pruned_project;
    }

    return root;
}

std::shared_ptr<LogicalKnnScan> BoundSelectStatement::BuildInitialKnnScan(std::shared_ptr<TableRef> &table_ref,
                                                                          std::shared_ptr<KnnExpression> knn_expr,
                                                                          QueryContext *query_context,
                                                                          const std::shared_ptr<BindContext> &bind_context) {
    if (table_ref.get() == nullptr) {
        UnrecoverableError("Attempt to do KNN scan without table");
    }
    switch (table_ref->type_) {
        case TableRefType::kCrossProduct: {
            UnrecoverableError("KNN is not supported on CROSS PRODUCT relation, now.");
            break;
        }
        case TableRefType::kJoin: {
            UnrecoverableError("KNN is not supported on JOIN relation, now.");
        }
        case TableRefType::kTable: {
            auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref);
            // Change function table to knn table scan function
            std::shared_ptr<LogicalKnnScan> knn_scan_node = std::make_shared<LogicalKnnScan>(bind_context->GetNewLogicalNodeId(),
                                                                                             std::move(base_table_ref),
                                                                                             std::move(knn_expr),
                                                                                             bind_context->knn_table_index_);
            return knn_scan_node;
        }
        case TableRefType::kSubquery: {
            UnrecoverableError("KNN is not supported on a SUBQUERY, now.");
            break;
        }
        default: {
            UnrecoverableError("Unexpected table type");
        }
    }

    return nullptr;
}

std::shared_ptr<LogicalNode>
BoundSelectStatement::BuildFrom(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context) {
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
                UnrecoverableError("Unknown table reference type.");
            }
        }
    } else {
        // No FROM-Clause, only select constant expression.
    }

    return nullptr;
}

std::shared_ptr<LogicalNode>
BoundSelectStatement::BuildBaseTable(std::shared_ptr<TableRef> &table_ref, QueryContext *, const std::shared_ptr<BindContext> &bind_context) {
    // std::shared_ptr<BaseTableRef> base_table_ref
    auto base_table_ref = static_pointer_cast<BaseTableRef>(table_ref);

    std::shared_ptr<LogicalTableScan> table_scan_node = std::make_shared<LogicalTableScan>(bind_context->GetNewLogicalNodeId(), base_table_ref);
    return table_scan_node;
}

std::shared_ptr<LogicalNode>
BoundSelectStatement::BuildSubqueryTable(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &) {
    // std::shared_ptr<SubqueryTableRef> subquery_table_ref
    auto subquery_table_ref = static_pointer_cast<SubqueryTableRef>(table_ref);
    std::shared_ptr<LogicalNode> subquery = subquery_table_ref->subquery_node_->BuildPlan(query_context);
    return subquery;
}

std::shared_ptr<LogicalNode> BoundSelectStatement::BuildCrossProductTable(std::shared_ptr<TableRef> &table_ref,
                                                                          QueryContext *query_context,
                                                                          const std::shared_ptr<BindContext> &bind_context) {
    // std::shared_ptr<CrossProductTableRef> cross_product_table_ref
    auto cross_product_table_ref = static_pointer_cast<CrossProductTableRef>(table_ref);

    auto left_node = BuildFrom(cross_product_table_ref->left_table_ref_, query_context, bind_context);
    auto right_node = BuildFrom(cross_product_table_ref->right_table_ref_, query_context, bind_context);

    // TODO: Merge bind context ?
    u64 logical_node_id = bind_context->GetNewLogicalNodeId();
    std::string alias(fmt::format("cross_product{}", logical_node_id));
    std::shared_ptr<LogicalCrossProduct> logical_cross_product_node =
        std::make_shared<LogicalCrossProduct>(logical_node_id, alias, left_node, right_node);
    return logical_cross_product_node;
}

std::shared_ptr<LogicalNode> BoundSelectStatement::BuildJoinTable(std::shared_ptr<TableRef> &table_ref,
                                                                  QueryContext *query_context,
                                                                  const std::shared_ptr<BindContext> &bind_context) {
    // std::shared_ptr<JoinTableRef> join_table_ref
    auto join_table_ref = static_pointer_cast<JoinTableRef>(table_ref);

    auto left_node = BuildFrom(join_table_ref->left_table_ref_, query_context, bind_context);
    auto right_node = BuildFrom(join_table_ref->right_table_ref_, query_context, bind_context);

    // TODO: Merge bind context ?
    u64 logical_node_id = bind_context->GetNewLogicalNodeId();
    std::string alias(fmt::format("join{}", logical_node_id));
    std::shared_ptr<LogicalJoin> logical_join_node =
        std::make_shared<LogicalJoin>(logical_node_id, join_table_ref->join_type_, alias, join_table_ref->on_conditions_, left_node, right_node);
    return logical_join_node;
}

std::shared_ptr<LogicalNode>
BoundSelectStatement::BuildDummyTable(std::shared_ptr<TableRef> &, QueryContext *, const std::shared_ptr<BindContext> &bind_context) {
    u64 logical_node_id = bind_context->GetNewLogicalNodeId();
    std::string alias(fmt::format("DummyTable{}", logical_node_id));
    std::shared_ptr<LogicalDummyScan> dummy_scan_node =
        std::make_shared<LogicalDummyScan>(logical_node_id, alias, bind_context->GenerateTableIndex());
    return dummy_scan_node;
}

std::shared_ptr<LogicalNode> BoundSelectStatement::BuildFilter(std::shared_ptr<LogicalNode> &root,
                                                               std::vector<std::shared_ptr<BaseExpression>> &conditions,
                                                               QueryContext *query_context,
                                                               const std::shared_ptr<BindContext> &bind_context) {
    for (auto &cond : conditions) {
        // 1. Go through all the expression to find subquery
        //        VisitExpression(cond,
        //                        [&](std::shared_ptr<BaseExpression> &expr) {
        //                            SubqueryUnnest::UnnestSubqueries(expr, root, bind_context);
        //                        });
        BuildSubquery(root, cond, query_context, bind_context);
    }

    // std::shared_ptr<BaseExpression> filter_expr
    auto filter_expr = ComposeExpressionWithDelimiter(conditions, ConjunctionType::kAnd);

    // std::shared_ptr<LogicalFilter> filter
    auto filter = std::make_shared<LogicalFilter>(bind_context->GetNewLogicalNodeId(), filter_expr);

    return filter;
}

std::shared_ptr<LogicalNode> BoundSelectStatement::BuildUnnest(std::shared_ptr<LogicalNode> &root,
                                                               std::vector<std::shared_ptr<BaseExpression>> &expressions,
                                                               QueryContext *query_context,
                                                               const std::shared_ptr<BindContext> &bind_context) {
    // std::shared_ptr<LogicalUnnest> unnest
    expressions = {bind_context->unnest_exprs_};
    size_t unnest_idx = bind_context->unnest_table_index_;
    auto unnest = std::make_shared<LogicalUnnest>(bind_context->GetNewLogicalNodeId(), expressions, unnest_idx);
    return unnest;
}

void BoundSelectStatement::BuildSubquery(std::shared_ptr<LogicalNode> &root,
                                         std::shared_ptr<BaseExpression> &condition,
                                         QueryContext *query_context,
                                         const std::shared_ptr<BindContext> &bind_context) {
    if (condition.get() == nullptr) {
        return;
    }

    VisitExpression(condition, [&](std::shared_ptr<BaseExpression> &expr) { BuildSubquery(root, expr, query_context, bind_context); });

    if (condition->type() == ExpressionType::kSubQuery) {
        if (building_subquery_) {
            // nested subquery
            UnrecoverableError("Nested subquery detected");
        }
        condition = UnnestSubquery(root, condition, query_context, bind_context);
    }
}

std::shared_ptr<BaseExpression> BoundSelectStatement::UnnestSubquery(std::shared_ptr<LogicalNode> &root,
                                                                     std::shared_ptr<BaseExpression> &condition,
                                                                     QueryContext *query_context,
                                                                     const std::shared_ptr<BindContext> &) {
    building_subquery_ = true;
    SubqueryExpression *subquery_expr_ptr = (SubqueryExpression *)condition.get();
    std::shared_ptr<LogicalNode> subquery_plan = subquery_expr_ptr->bound_select_statement_ptr_->BuildPlan(query_context);
    std::shared_ptr<BaseExpression> return_expr = nullptr;
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
