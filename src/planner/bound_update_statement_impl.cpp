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

module infinity_core:bound_update_statement.impl;

import :bound_update_statement;
import :bound_statement;
import :table_ref;
import :base_expression;
import :bind_context;
import :logical_node;
import :query_context;
import :infinity_exception;
import :status;
import :base_table_ref;
import :subquery_table_ref;
import :expression_transformer;
import :expression_type;
import :subquery_expression;
import :query_context;
import :logical_table_scan;
import :logical_filter;
import :logical_update;
import :subquery_unnest;
import :conjunction_expression;

import std;

import table_reference;

namespace infinity {

std::shared_ptr<LogicalNode> BoundUpdateStatement::BuildPlan(QueryContext *query_context) {
    const std::shared_ptr<BindContext> &bind_context = this->bind_context_;
    std::shared_ptr<LogicalNode> current_node;
    if (where_conditions_.empty()) {
        Status status = Status::SyntaxError("where_conditions_ shall not be empty");
        RecoverableError(status);
    }
    std::shared_ptr<LogicalNode> table_scan_node = BuildFrom(table_ref_ptr_, query_context, bind_context);
    if (!where_conditions_.empty()) {
        std::shared_ptr<LogicalNode> filter_node = BuildFilter(table_scan_node, where_conditions_, query_context, bind_context);
        filter_node->set_left_node(table_scan_node);
        current_node = filter_node;
    } else {
        current_node = table_scan_node;
    }

    auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref_ptr_);
    auto update_node = std::make_shared<LogicalUpdate>(bind_context->GetNewLogicalNodeId(),
                                                 base_table_ref->table_info_,
                                                 update_columns_,
                                                 all_columns_in_table_,
                                                 final_result_columns_);
    update_node->set_left_node(current_node);
    return update_node;
}

std::shared_ptr<LogicalNode>
BoundUpdateStatement::BuildFrom(std::shared_ptr<TableRef> &table_ref, QueryContext *query_context, const std::shared_ptr<BindContext> &bind_context) {
    if (table_ref.get() == nullptr || table_ref->type_ != TableRefType::kTable) {
        UnrecoverableError("Unsupported");
    }
    return BuildBaseTable(table_ref, query_context, bind_context);
}

std::shared_ptr<LogicalNode>
BoundUpdateStatement::BuildBaseTable(std::shared_ptr<TableRef> &table_ref, QueryContext *, const std::shared_ptr<BindContext> &bind_context) {
    // std::shared_ptr<BaseTableRef> base_table_ref
    auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref);

    std::shared_ptr<LogicalTableScan> table_scan_node = std::make_shared<LogicalTableScan>(bind_context->GetNewLogicalNodeId(), base_table_ref, true);
    return table_scan_node;
}

std::shared_ptr<LogicalNode> BoundUpdateStatement::BuildFilter(std::shared_ptr<LogicalNode> &root,
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

void BoundUpdateStatement::BuildSubquery(std::shared_ptr<LogicalNode> &root,
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
            Status status = Status::SyntaxError("Nested subquery detected");
            RecoverableError(status);
        }
        condition = UnnestSubquery(root, condition, query_context, bind_context);
    }
}

std::shared_ptr<BaseExpression> BoundUpdateStatement::UnnestSubquery(std::shared_ptr<LogicalNode> &root,
                                                               std::shared_ptr<BaseExpression> &condition,
                                                               QueryContext *query_context,
                                                               const std::shared_ptr<BindContext> &) {
    building_subquery_ = true;
    //    std::unique_ptr<QueryBinder> query_binder_ptr = std::make_unique<QueryBinder>(query_context,
    //                                                                      bind_context);
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
