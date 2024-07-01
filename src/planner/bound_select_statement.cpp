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

#include <string>
#include <cstdlib>

module bound_select_statement;

import logical_node;
import stl;
import query_context;
import bind_context;
import table_ref;
import base_expression;
import conjunction_expression;
import subquery_expression;

import logical_node;
import logical_node_type;
import logical_create_schema;
import logical_create_view;
import logical_create_table;
import logical_create_collection;
import logical_drop_schema;
import logical_drop_table;
import logical_drop_collection;
import logical_drop_view;
import logical_flush;
import logical_insert;
import logical_project;
import logical_filter;
import logical_table_scan;
import logical_knn_scan;
import logical_match_tensor_scan;
import logical_match_sparse_scan;
import logical_aggregate;
import logical_sort;
import logical_limit;
import logical_top;
import logical_cross_product;
import logical_join;
import logical_show;
import logical_export;
import logical_import;
import logical_dummy_scan;
import logical_match;
import logical_fusion;

import subquery_unnest;

import infinity_exception;
import expression_transformer;
import expression_type;

import base_table_ref;
import subquery_table_ref;
import cross_product_table_ref;
import join_table_ref;
import knn_expression;
import match_expression;
import match_tensor_expression;
import match_sparse_expression;
import third_party;
import table_reference;
import common_query_filter;
import logger;

import search_options;
import search_driver;
import query_node;
import doc_iterator;
import status;
import default_values;

namespace infinity {

SharedPtr<LogicalNode> BoundSelectStatement::BuildPlan(QueryContext *query_context) {
    const SharedPtr<BindContext> &bind_context = this->bind_context_;
    if (search_expr_.get() == nullptr) {
        SharedPtr<LogicalNode> root = BuildFrom(table_ref_ptr_, query_context, bind_context);
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
                                                                    order_by_types_);
                top->set_left_node(root);
                root = top;
            }
        } else if (limit_expression_.get() != nullptr) {
            auto limit = MakeShared<LogicalLimit>(bind_context->GetNewLogicalNodeId(), limit_expression_, offset_expression_);
            limit->set_left_node(root);
            root = limit;
        }

        auto project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(), projection_expressions_, projection_index_);
        project->set_left_node(root);
        root = project;

        if (!pruned_expression_.empty()) {
            String error_message = "Projection method changed!";
            UnrecoverableError(error_message);
            auto pruned_project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(), pruned_expression_, result_index_);
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
        auto filter_expr = ComposeExpressionWithDelimiter(where_conditions_, ConjunctionType::kAnd);
        auto common_query_filter = MakeShared<CommonQueryFilter>(filter_expr, base_table_ref, query_context->GetTxn()->BeginTS());
        Vector<SharedPtr<LogicalNode>> match_knn_nodes;
        match_knn_nodes.reserve(num_children);
        for (auto &match_expr : search_expr_->match_exprs_) {
            switch (match_expr->type()) {
                case ExpressionType::kMatch: {
                    SharedPtr<LogicalMatch> match_node = MakeShared<LogicalMatch>(bind_context->GetNewLogicalNodeId(),
                                                                                  base_table_ref,
                                                                                  std::dynamic_pointer_cast<MatchExpression>(match_expr));
                    match_node->filter_expression_ = filter_expr;
                    match_node->common_query_filter_ = common_query_filter;
                    match_node->index_reader_ = base_table_ref->table_entry_ptr_->GetFullTextIndexReader(query_context->GetTxn());

                    const Map<String, String> &column2analyzer = match_node->index_reader_.GetColumn2Analyzer();
                    SearchOptions search_ops(match_node->match_expr_->options_text_);

                    // option: threshold
                    const String &threshold = search_ops.options_["threshold"];
                    match_node->begin_threshold_ = strtof(threshold.c_str(), nullptr);

                    // option: default field
                    auto iter = search_ops.options_.find("default_field");
                    String default_field;
                    if (iter != search_ops.options_.end()) {
                        default_field = iter->second;
                    }

                    // option: block max
                    iter = search_ops.options_.find("block_max");
                    if (iter == search_ops.options_.end() or iter->second == "true" or iter->second == "bmw") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kBMW;
                    } else if (iter->second == "bmm") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kBMM;
                    } else if (iter->second == "false") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kNaive;
                    } else if (iter->second == "compare") {
                        match_node->early_term_algo_ = EarlyTermAlgo::kCompare;
                    } else {
                        Status status = Status::SyntaxError("block_max option must be empty, true, false or compare");
                        RecoverableError(status);
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

                    SearchDriver search_driver(column2analyzer, default_field);
                    UniquePtr<QueryNode> query_tree =
                        search_driver.ParseSingleWithFields(match_node->match_expr_->fields_, match_node->match_expr_->matching_text_);
                    if (query_tree.get() == nullptr) {
                        Status status = Status::ParseMatchExprFailed(match_node->match_expr_->fields_, match_node->match_expr_->matching_text_);
                        RecoverableError(status);
                    }

                    match_node->query_tree_ = std::move(query_tree);
                    match_knn_nodes.push_back(std::move(match_node));
                    break;
                }
                case ExpressionType::kKnn: {
                    SharedPtr<LogicalKnnScan> knn_scan =
                        BuildInitialKnnScan(table_ref_ptr_, std::dynamic_pointer_cast<KnnExpression>(match_expr), query_context, bind_context);
                    knn_scan->filter_expression_ = filter_expr;
                    knn_scan->common_query_filter_ = common_query_filter;
                    match_knn_nodes.push_back(std::move(knn_scan));
                    break;
                }
                case ExpressionType::kMatchTensor: {
                    auto match_tensor_node = MakeShared<LogicalMatchTensorScan>(bind_context->GetNewLogicalNodeId(),
                                                                                base_table_ref,
                                                                                std::dynamic_pointer_cast<MatchTensorExpression>(match_expr));
                    match_tensor_node->filter_expression_ = filter_expr;
                    match_tensor_node->common_query_filter_ = common_query_filter;
                    match_tensor_node->InitExtraOptions();
                    match_knn_nodes.push_back(std::move(match_tensor_node));
                    break;
                }
                case ExpressionType::kMatchSparse: {
                    auto match_sparse_node = MakeShared<LogicalMatchSparseScan>(bind_context->GetNewLogicalNodeId(),
                                                                                base_table_ref,
                                                                                std::dynamic_pointer_cast<MatchSparseExpression>(match_expr));
                    match_sparse_node->filter_expression_ = filter_expr;
                    match_sparse_node->common_query_filter_ = common_query_filter;
                    match_knn_nodes.push_back(std::move(match_sparse_node));
                    break;
                }
                default: {
                    String error_message = "Unsupported match expression type";
                    UnrecoverableError(error_message);
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

        auto project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(), projection_expressions_, projection_index_);
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
            auto base_table_ref = static_pointer_cast<BaseTableRef>(table_ref);

            // Change function table to knn table scan function
            SharedPtr<LogicalKnnScan> knn_scan_node =
                MakeShared<LogicalKnnScan>(bind_context->GetNewLogicalNodeId(), base_table_ref, knn_expr, bind_context->knn_table_index_);
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
