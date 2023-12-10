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

import stl;
import parser;
import logical_node;
import logical_node_type;
import third_party;

import infinity_exception;
import logical_aggregate;
import logical_join;
import logical_table_scan;
import logical_cross_product;
import logical_project;

import function_expression;
import base_expression;
import column_expression;

import binding;
import base_table_ref;

import new_catalog;
import function_set;
import scalar_function;
import scalar_function_set;
import table_scan;
import corrlated_expr_detector;
import rewrite_correlated_expression;

module dependent_join_flattener;

namespace infinity {

bool DependentJoinFlattener::DetectCorrelatedExpressions(const SharedPtr<LogicalNode> &logical_node) {
    CorrelatedExpressionsDetector detector(bind_context_ptr_->correlated_column_exprs_);

    detector.VisitNode(*logical_node);

    bool is_correlated = detector.IsCorrelated();

    // Check child.
    if (logical_node->left_node().get() != nullptr) {
        if (DetectCorrelatedExpressions(logical_node->left_node())) {
            is_correlated = true;
        }
    }
    if (logical_node->right_node().get() != nullptr) {
        if (DetectCorrelatedExpressions(logical_node->right_node())) {
            is_correlated = true;
        }
    }

    operator2correlated_expression_map_[logical_node->node_id()] = is_correlated;
    return is_correlated;
}

SharedPtr<LogicalNode> DependentJoinFlattener::PushDependentJoin(const SharedPtr<LogicalNode> &logical_node) {
    return PushDependentJoinInternal(logical_node);
}

SharedPtr<LogicalNode> DependentJoinFlattener::PushDependentJoinInternal(const SharedPtr<LogicalNode> &subquery_plan) {
    // 1. Validates if the logical node was checked in operator2correlated_expression_map_ before.
    if (!operator2correlated_expression_map_.contains(subquery_plan->node_id())) {
        Error<PlannerException>(Format("Logical node {} wasn't detected before.", subquery_plan->node_id()));
    }

    // 2. if no correlated expression in this operator. which means all correlated expressions are unnested
    if (!operator2correlated_expression_map_[subquery_plan->node_id()]) {
        return BuildNoCorrelatedInternal(subquery_plan);
    }
    switch (subquery_plan->operator_type()) {
        case LogicalNodeType::kAggregate: {
            // Push down the dependent join
            auto pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());
            subquery_plan->set_left_node(pushed_plan);

            // then we replace any correlated expressions with the corresponding entry in the correlated_map
            RewriteCorrelatedExpressions rewriter(bind_context_ptr_, base_binding_);
            rewriter.VisitNode(*subquery_plan);

            // Push the correlated column to the group by list
            LogicalAggregate *aggregate_node = (LogicalAggregate *)subquery_plan.get();
            SizeT column_count = bind_context_ptr_->correlated_column_exprs_.size();
            aggregate_node->groups_.reserve(aggregate_node->groups_.size() + column_count);
            for (SizeT idx = 0; idx < column_count; ++idx) {
                SharedPtr<ColumnExpression> correlated_column = bind_context_ptr_->correlated_column_exprs_[idx];
                SharedPtr<ColumnExpression> new_column = ColumnExpression::Make(correlated_column->Type(),
                                                                                correlated_column->table_name(),
                                                                                base_binding_.table_idx,
                                                                                correlated_column->column_name(),
                                                                                base_binding_.column_idx + idx,
                                                                                0);
                aggregate_node->groups_.emplace_back(new_column);
            }

            // Update parent table information
            base_binding_.table_idx = aggregate_node->groupby_index_;
            base_binding_.column_idx = aggregate_node->groups_.size() - column_count;

            correlated_expression_offset_ = base_binding_.column_idx;

            return subquery_plan;
        }
        case LogicalNodeType::kExcept:
        case LogicalNodeType::kUnion:
        case LogicalNodeType::kIntersect: {
            Error<PlannerException>("Can't push down through set operation node.");
            break;
        }
        case LogicalNodeType::kJoin: {
            Error<PlannerException>("Can't push down through join node.");
            break;
        }
        case LogicalNodeType::kCrossProduct: {
            bool left_has_correlated_expr = operator2correlated_expression_map_.contains(subquery_plan->left_node()->node_id());
            bool right_has_correlated_expr = operator2correlated_expression_map_.contains(subquery_plan->right_node()->node_id());

            if (!right_has_correlated_expr) {
                auto pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());
                subquery_plan->set_left_node(pushed_plan);
                return subquery_plan;
            }
            if (!left_has_correlated_expr) {
                auto pushed_plan = PushDependentJoinInternal(subquery_plan->right_node());
                subquery_plan->set_right_node(pushed_plan);
                return subquery_plan;
            }

            // Both sides have correlated expression
            auto left_pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());
            auto left_correlated_binding = this->base_binding_;
            auto right_pushed_plan = PushDependentJoinInternal(subquery_plan->right_node());
            auto right_correlated_binding = this->base_binding_;

            NewCatalog *catalog = query_context_->storage()->catalog();
            SharedPtr<FunctionSet> function_set_ptr = NewCatalog::GetFunctionSetByName(catalog, "=");
            Vector<SharedPtr<BaseExpression>> join_conditions;

            SizeT column_count = bind_context_ptr_->correlated_column_exprs_.size();
            join_conditions.reserve(column_count);
            for (SizeT idx = 0; idx < column_count; ++idx) {
                SizeT left_correlated_column_index = left_correlated_binding.column_idx + idx;
                SizeT right_correlated_column_index = right_correlated_binding.column_idx + idx;

                SharedPtr<ColumnExpression> left_column_expr = ColumnExpression::Make(bind_context_ptr_->correlated_column_exprs_[idx]->Type(),
                                                                                      bind_context_ptr_->correlated_column_exprs_[idx]->table_name(),
                                                                                      left_correlated_binding.table_idx,
                                                                                      bind_context_ptr_->correlated_column_exprs_[idx]->column_name(),
                                                                                      left_correlated_column_index,
                                                                                      0);

                SharedPtr<ColumnExpression> right_column_expr =
                    ColumnExpression::Make(bind_context_ptr_->correlated_column_exprs_[idx]->Type(),
                                           bind_context_ptr_->correlated_column_exprs_[idx]->table_name(),
                                           right_correlated_binding.table_idx,
                                           bind_context_ptr_->correlated_column_exprs_[idx]->column_name(),
                                           right_correlated_column_index,
                                           0);

                // Generate join condition expression
                Vector<SharedPtr<BaseExpression>> function_arguments;
                function_arguments.reserve(2);
                function_arguments.emplace_back(left_column_expr);
                function_arguments.emplace_back(right_column_expr);

                auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
                ScalarFunction equi_function = scalar_function_set_ptr->GetMostMatchFunction(function_arguments);

                SharedPtr<FunctionExpression> function_expr_ptr = MakeShared<FunctionExpression>(equi_function, function_arguments);
                join_conditions.emplace_back(function_expr_ptr);
            }

            u64 logical_node_id = bind_context_ptr_->GetNewLogicalNodeId();
            String alias = "logical_join";
            alias += ToStr(logical_node_id);
            SharedPtr<LogicalJoin> logical_join = MakeShared<LogicalJoin>(logical_node_id,
                                                                          JoinType::kInner,
                                                                          alias,
                                                                          join_conditions,
                                                                          subquery_plan->left_node(),
                                                                          subquery_plan->right_node());

            return logical_join;
        }
        case LogicalNodeType::kLimit: {
            Error<PlannerException>("Can't push down through limit node");
            break;
        }
        case LogicalNodeType::kFilter: {

            // Push down the dependent join
            auto pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());
            subquery_plan->set_left_node(pushed_plan);

            // then we replace any correlated expressions with the corresponding entry in the correlated_map
            RewriteCorrelatedExpressions rewriter(bind_context_ptr_, base_binding_);
            rewriter.VisitNode(*subquery_plan);
            return subquery_plan;
        }
        case LogicalNodeType::kProjection: {

            // Push down the dependent join
            auto pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());
            subquery_plan->set_left_node(pushed_plan);

            // then we replace any correlated expressions with the corresponding entry in the correlated_map
            RewriteCorrelatedExpressions rewriter(bind_context_ptr_, base_binding_);
            rewriter.VisitNode(*subquery_plan);

            // Push the correlated column to the project list
            LogicalProject *project_node = (LogicalProject *)subquery_plan.get();
            SizeT column_count = bind_context_ptr_->correlated_column_exprs_.size();
            project_node->expressions_.reserve(project_node->expressions_.size() + column_count);
            for (SizeT idx = 0; idx < column_count; ++idx) {
                SharedPtr<ColumnExpression> correlated_column = bind_context_ptr_->correlated_column_exprs_[idx];
                SharedPtr<ColumnExpression> new_column = ColumnExpression::Make(correlated_column->Type(),
                                                                                correlated_column->table_name(),
                                                                                base_binding_.table_idx,
                                                                                correlated_column->column_name(),
                                                                                base_binding_.column_idx + idx,
                                                                                0);
                project_node->expressions_.emplace_back(new_column);
            }

            // Update parent table information
            base_binding_.table_idx = project_node->table_index_;
            base_binding_.column_idx = project_node->expressions_.size() - column_count;
            correlated_expression_offset_ = base_binding_.column_idx;
            return subquery_plan;
        }
        case LogicalNodeType::kSort: {
            Error<PlannerException>("Can't push down through order by node");
            break;
        }
        case LogicalNodeType::kTableScan: {
            Error<PlannerException>("Can't push down through table scan node");
            break;
        }
        case LogicalNodeType::kDelete:
        case LogicalNodeType::kUpdate:
        case LogicalNodeType::kInsert:
        case LogicalNodeType::kImport:
        case LogicalNodeType::kExport:
        case LogicalNodeType::kAlter:
        case LogicalNodeType::kCreateTable:
        case LogicalNodeType::kCreateCollection:
        case LogicalNodeType::kCreateSchema:
        case LogicalNodeType::kCreateView:
        case LogicalNodeType::kDropTable:
        case LogicalNodeType::kDropCollection:
        case LogicalNodeType::kDropSchema:
        case LogicalNodeType::kDropView:
        case LogicalNodeType::kViewScan:
        case LogicalNodeType::kKnnScan:
        case LogicalNodeType::kDummyScan:
        case LogicalNodeType::kShow:
        case LogicalNodeType::kExplain:
        case LogicalNodeType::kPrepare: {
            Error<PlannerException>(Format("Logical node {} should be involved in subquery.", subquery_plan->name()));
        }
        case LogicalNodeType::kInvalid: {
            Error<PlannerException>("Invalid logical operator node");
        }
        default: {
            Error<PlannerException>("Unsupported logical operator node");
        }
    }
    Error<PlannerException>("Unreachable");
    return nullptr;
}

SharedPtr<LogicalNode> DependentJoinFlattener::BuildNoCorrelatedInternal(const SharedPtr<LogicalNode> &subquery_plan) {
    const Vector<SharedPtr<ColumnExpression>> &correlated_columns = bind_context_ptr_->correlated_column_exprs_;

    // Get the correlated column and generate table scan
    SharedPtr<Vector<String>> column_names = MakeShared<Vector<String>>();
    SharedPtr<Vector<SharedPtr<DataType>>> column_types = MakeShared<Vector<SharedPtr<DataType>>>();
    Vector<SizeT> column_ids;

    SizeT column_count = correlated_columns.size();
    column_names->reserve(column_count);
    column_types->reserve(column_count);
    column_ids.reserve(column_count);

    SizeT table_index = correlated_columns[0]->binding().table_idx;
    column_names->emplace_back(correlated_columns[0]->column_name());
    column_types->emplace_back(MakeShared<DataType>(correlated_columns[0]->Type()));
    column_ids.emplace_back(correlated_columns[0]->binding().column_idx);
    for (SizeT idx = 1; idx < column_count; ++idx) {
        Assert<PlannerException>(correlated_columns[idx]->binding().table_idx == table_index, "Correlated column are from different table.");
        column_names->emplace_back(correlated_columns[idx]->column_name());
        column_types->emplace_back(MakeShared<DataType>(correlated_columns[idx]->Type()));
        column_ids.emplace_back(correlated_columns[idx]->binding().column_idx);
    }

    const Binding *table_binding_ptr = bind_context_ptr_->GetBindingFromCurrentOrParentByName(correlated_columns[0]->table_name());
    if (table_binding_ptr == nullptr) {
        Error<PlannerException>(Format("Can't find table: {} in binding context.", correlated_columns[0]->table_name()));
    }

    NewCatalog *catalog = query_context_->storage()->catalog();

    SharedPtr<BaseTableRef> base_table_ref = MakeShared<BaseTableRef>(table_binding_ptr->table_collection_entry_ptr_,
                                                                      column_ids,
                                                                      table_binding_ptr->block_index_,
                                                                      table_binding_ptr->table_name_,
                                                                      table_index,
                                                                      column_names,
                                                                      column_types);

    SharedPtr<LogicalTableScan> logical_table_scan = MakeShared<LogicalTableScan>(bind_context_ptr_->GetNewLogicalNodeId(), base_table_ref);

    // Generate cross product
    u64 logical_node_id = bind_context_ptr_->GetNewLogicalNodeId();
    String alias = "cross_product";
    alias += ToStr(logical_node_id);
    SharedPtr<LogicalCrossProduct> cross_product_node = MakeShared<LogicalCrossProduct>(logical_node_id, alias, subquery_plan, logical_table_scan);

    this->base_binding_.table_idx = table_index;
    this->base_binding_.column_idx = 0;

    this->correlated_expression_offset_ = subquery_plan->GetOutputNames()->size();

    return cross_product_node;
}

} // namespace infinity
