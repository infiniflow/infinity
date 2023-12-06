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

#include <memory>

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
import logical_aggregate;
import logical_sort;
import logical_limit;
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
import knn_scan;

import parser;
import base_table_ref;
import subquery_table_ref;
import cross_product_table_ref;
import join_table_ref;

module bound_select_statement;

namespace infinity {

SharedPtr<LogicalNode> BoundSelectStatement::BuildPlan(QueryContext *query_context) {
    const SharedPtr<BindContext> &bind_context = this->bind_context_;
    if (search_expr_ == nullptr && bind_context->knn_exprs_.empty()) {
        // Non-knn case
        SharedPtr<LogicalNode> root = BuildFrom(table_ref_ptr_, query_context, bind_context);
        if (!where_conditions_.empty()) {
            SharedPtr<LogicalNode> filter = BuildFilter(root, where_conditions_, query_context, bind_context);
            filter->set_left_node(root);
            root = filter;
        }

        if (!group_by_expressions_.empty() || !aggregate_expressions_.empty()) {
            // Build logical aggregate
            auto aggregate = MakeShared<LogicalAggregate>(bind_context->GetNewLogicalNodeId(),
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

        auto project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(), projection_expressions_, projection_index_);
        project->set_left_node(root);
        root = project;

        if (!order_by_expressions_.empty()) {
            Assert<PlannerException>(order_by_expressions_.size() == order_by_types_.size(), "Unknown error on order by expression");
            SharedPtr<LogicalNode> sort = MakeShared<LogicalSort>(bind_context->GetNewLogicalNodeId(), order_by_expressions_, order_by_types_);
            sort->set_left_node(root);
            root = sort;
        }

        if (limit_expression_.get() != nullptr) {
            SharedPtr<LogicalLimit> limit = MakeShared<LogicalLimit>(bind_context->GetNewLogicalNodeId(), limit_expression_, offset_expression_);
            limit->set_left_node(root);
            root = limit;
        }

        if (!pruned_expression_.empty()) {
            auto pruned_project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(), pruned_expression_, result_index_);
            pruned_project->set_left_node(root);
            root = pruned_project;
        }

        return root;
    } else if (!bind_context->knn_exprs_.empty()) {
        SharedPtr<LogicalNode> root = nullptr;

        // Knn case
        SharedPtr<LogicalKnnScan> knn_scan = BuildInitialKnnScan(table_ref_ptr_, query_context, bind_context);
        if (bind_context_->knn_orders_.size() != 1) {
            Error<PlannerException>("Knn Scan need order by clause which should have only one expression");
        }

        knn_scan->order_by_type_ = bind_context_->knn_orders_[0];
        knn_scan->limit_expression_ = limit_expression_;

        // FIXME: need check if there is subquery inside the where conditions
        auto filter_expr = ComposeExpressionWithDelimiter(where_conditions_, ConjunctionType::kAnd);

        knn_scan->filter_expression_ = filter_expr;
        root = knn_scan;

        auto project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(), projection_expressions_, projection_index_);
        project->set_left_node(root);
        root = project;

        return root;
    } else {
        SharedPtr<LogicalNode> root = nullptr;
        // FIXME: add KNN support here!
        if (search_expr_->match_exprs_.empty()) {
            Error<PlannerException>("SEARCH shall have at least one MATCH expression");
        }
        if (search_expr_->match_exprs_.size() >= 3) {
            Error<PlannerException>("SEARCH shall have at max two MATCH expression");
        }

        Vector<SharedPtr<LogicalNode>> match_knn_nodes;
        match_knn_nodes.reserve(search_expr_->match_exprs_.size());
        for (auto &match_expr : search_expr_->match_exprs_) {
            Assert<PlannerException>(table_ref_ptr_->type() == TableRefType::kTable, "Not base table reference");
            auto base_table_ref = static_pointer_cast<BaseTableRef>(table_ref_ptr_);
            SharedPtr<LogicalNode> matchNode = MakeShared<LogicalMatch>(bind_context->GetNewLogicalNodeId(), base_table_ref, match_expr);
            match_knn_nodes.push_back(matchNode);
            root = matchNode;
        }

        if (search_expr_->fusion_expr_ != nullptr) {
            SharedPtr<LogicalNode> fusionNode = MakeShared<LogicalFusion>(bind_context->GetNewLogicalNodeId(),
                                                                          search_expr_->fusion_expr_,
                                                                          match_knn_nodes[0]->GetColumnBindings(),
                                                                          match_knn_nodes[0]->GetOutputNames(),
                                                                          match_knn_nodes[0]->GetOutputTypes());
            fusionNode->set_left_node(match_knn_nodes[0]);
            if (match_knn_nodes.size() > 1)
                fusionNode->set_right_node(match_knn_nodes[1]);
            root = fusionNode;
        }

        auto project = MakeShared<LogicalProject>(bind_context->GetNewLogicalNodeId(), projection_expressions_, projection_index_);
        project->set_left_node(root);
        root = project;

        return root;
    }
}

SharedPtr<LogicalKnnScan>
BoundSelectStatement::BuildInitialKnnScan(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context) {
    if (table_ref.get() == nullptr) {
        Error<PlannerException>("Attempt to do KNN scan without table");
    }
    switch (table_ref->type_) {
        case TableRefType::kCrossProduct: {
            Error<PlannerException>("KNN is not supported on CROSS PRODUCT relation, now.");
            break;
        }
        case TableRefType::kJoin: {
            Error<PlannerException>("KNN is not supported on JOIN relation, now.");
        }
        case TableRefType::kTable: {
            auto base_table_ref = static_pointer_cast<BaseTableRef>(table_ref);

            // Change function table to knn table scan function
            base_table_ref->table_func_ = KnnScanFunction::Make(query_context->storage()->catalog(), "knn_scan");

            SharedPtr<LogicalKnnScan> knn_scan_node = MakeShared<LogicalKnnScan>(bind_context->GetNewLogicalNodeId(), base_table_ref);

            knn_scan_node->knn_expressions_ = bind_context->knn_exprs_;
            knn_scan_node->knn_table_index_ = bind_context->knn_table_index_;
            return knn_scan_node;
        }
        case TableRefType::kSubquery: {
            Error<PlannerException>("KNN is not supported on a SUBQUERY, now.");
            break;
        }
        default: {
            Error<PlannerException>("Unexpected table type");
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
                Error<PlannerException>("Unknown table reference type.");
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
    String alias("cross_product" + ToStr(logical_node_id));
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
    String alias("join" + ToStr(logical_node_id));
    SharedPtr<LogicalJoin> logical_join_node =
        MakeShared<LogicalJoin>(logical_node_id, join_table_ref->join_type_, alias, join_table_ref->on_conditions_, left_node, right_node);
    return logical_join_node;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildDummyTable(SharedPtr<TableRef> &, QueryContext *, const SharedPtr<BindContext> &bind_context) {
    u64 logical_node_id = bind_context->GetNewLogicalNodeId();
    String alias("DummyTable" + ToStr(logical_node_id));
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
            Error<PlannerException>("Nested subquery detected");
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
