//
// Created by JinHai on 2022/9/24.
//

#include "dependent_join_flattener.h"
#include "correlated_expressions_detector.h"
#include "planner/node/logical_table_scan.h"
#include "planner/node/logical_cross_product.h"
#include "rewrite_correlated_expressions.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_aggregate.h"

namespace infinity {

bool
DependentJoinFlattener::DetectCorrelatedExpressions(const SharedPtr<LogicalNode>& logical_node) {

    CorrelatedExpressionsDetector detector(bind_context_ptr_->correlated_column_exprs_);

    detector.VisitNode(*logical_node);

    bool is_correlated = detector.IsCorrelated();

    // Check child.
    if(logical_node->left_node() != nullptr) {
        if(DetectCorrelatedExpressions(logical_node->left_node())) {
            is_correlated = true;
        }
    }
    if(logical_node->right_node() != nullptr) {
        if(DetectCorrelatedExpressions(logical_node->right_node())) {
            is_correlated = true;
        }
    }

    operator2correlated_expression_map_[logical_node->node_id()] = is_correlated;
    return true;
}

SharedPtr<LogicalNode>
DependentJoinFlattener::PushDependentJoin(const SharedPtr<LogicalNode>& logical_node) {
    return PushDependentJoinInternal(logical_node);
}

SharedPtr<LogicalNode>
DependentJoinFlattener::PushDependentJoinInternal(const SharedPtr<LogicalNode>& subquery_plan) {

    // 1. Validates if the logical node was checked in operator2correlated_expression_map_ before.
    if(!operator2correlated_expression_map_.contains(subquery_plan->node_id())) {
        PlannerError(fmt::format("Logical node {} wasn't detected before.", subquery_plan->node_id()));
    }

    // 2. if no correlated expression in this operator. which means all correlated expressions are unnested
    if(!operator2correlated_expression_map_[subquery_plan->node_id()]) {
        return BuildNoCorrelatedInternal(subquery_plan);
    }
    switch(subquery_plan->operator_type()) {
        case LogicalNodeType::kAggregate: {
            // Push down the dependent join
            auto pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());

            // then we replace any correlated expressions with the corresponding entry in the correlated_map
            RewriteCorrelatedExpressions rewriter(bind_context_ptr_,
                                                  parent_table_index_,
                                                  left_offset_,
                                                  right_offset_);
            rewriter.VisitNode(*subquery_plan);

            // Push the correlated column to the group by list
            LogicalAggregate* aggregate_node = (LogicalAggregate*)subquery_plan.get();
            SizeT column_count = bind_context_ptr_->correlated_column_exprs_.size();
            aggregate_node->groups_.reserve(aggregate_node->groups_.size() + column_count);
            for(SizeT idx = 0; idx < column_count; ++ idx) {
                SharedPtr<ColumnExpression> correlated_column = bind_context_ptr_->correlated_column_exprs_[idx];
                SharedPtr<ColumnExpression> new_column = ColumnExpression::Make(
                        correlated_column->Type(),
                        correlated_column->table_name(),
                        parent_table_index_,
                        correlated_column->column_name(),
                        right_offset_ + idx,
                        0);
                aggregate_node->groups_.emplace_back(new_column);
            }

            // Update parent table information
            parent_table_index_ = aggregate_node->groupby_index_;
            left_offset_ = 0;
            right_offset_ = column_count;

            return subquery_plan;
        }
        case LogicalNodeType::kExcept:
        case LogicalNodeType::kUnion:
        case LogicalNodeType::kIntersect: {
            PlannerError("Can't push down through set operation node");
            break;
        }
        case LogicalNodeType::kJoin: {
            PlannerError("Can't push down through join node");
            break;
        }
        case LogicalNodeType::kCrossProduct: {
            PlannerError("Can't push down through cross product node");
            break;
        }
        case LogicalNodeType::kLimit: {
            PlannerError("Can't push down through limit node");
            break;
        }
        case LogicalNodeType::kFilter: {

            // Push down the dependent join
            auto pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());

            // then we replace any correlated expressions with the corresponding entry in the correlated_map
            RewriteCorrelatedExpressions rewriter(bind_context_ptr_,
                                                  parent_table_index_,
                                                  left_offset_,
                                                  right_offset_);
            rewriter.VisitNode(*subquery_plan);
            return subquery_plan;
        }
        case LogicalNodeType::kProjection: {

            // Push down the dependent join
            auto pushed_plan = PushDependentJoinInternal(subquery_plan->left_node());

            // then we replace any correlated expressions with the corresponding entry in the correlated_map
            RewriteCorrelatedExpressions rewriter(bind_context_ptr_,
                                                  parent_table_index_,
                                                  left_offset_,
                                                  right_offset_);
            rewriter.VisitNode(*subquery_plan);

            // Push the correlated column to the project list
            LogicalProject* project_node = (LogicalProject*)subquery_plan.get();
            SizeT column_count = bind_context_ptr_->correlated_column_exprs_.size();
            project_node->expressions_.reserve(project_node->expressions_.size() + column_count);
            for(SizeT idx = 0; idx < column_count; ++ idx) {
                SharedPtr<ColumnExpression> correlated_column = bind_context_ptr_->correlated_column_exprs_[idx];
                SharedPtr<ColumnExpression> new_column = ColumnExpression::Make(
                        correlated_column->Type(),
                        correlated_column->table_name(),
                        parent_table_index_,
                        correlated_column->column_name(),
                        right_offset_ + idx,
                        0);
                project_node->expressions_.emplace_back(new_column);
            }

            // Update parent table information
            parent_table_index_ = project_node->table_index_;
            left_offset_ = 0;
            right_offset_ = column_count;
            return subquery_plan;
        }
        case LogicalNodeType::kSort: {
            PlannerError("Can't push down through order by node");
            break;
        }
        case LogicalNodeType::kTableScan: {
            PlannerError("Can't push down through table scan node");
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
        case LogicalNodeType::kDummyScan:
        case LogicalNodeType::kShow:
        case LogicalNodeType::kExplain:
        case LogicalNodeType::kPrepare: {
            PlannerError(fmt::format("Logical node {} should be involved in subquery.", subquery_plan->name()));
        }
        case LogicalNodeType::kInvalid: {
            PlannerError("Invalid logical operator node");
        }
    }
    PlannerError("Unreachable");
}

SharedPtr<LogicalNode>
DependentJoinFlattener::BuildNoCorrelatedInternal(const SharedPtr<LogicalNode>& subquery_plan) {
    const Vector<SharedPtr<ColumnExpression>>& correlated_columns = bind_context_ptr_->correlated_column_exprs_;

    // Get the correlated column and generate table scan
    Vector<String> column_names;
    Vector<DataType> column_types;
    Vector<SizeT> column_ids;

    SizeT column_count = correlated_columns.size();
    column_names.reserve(column_count);
    column_types.reserve(column_count);
    column_ids.reserve(column_count);

    SizeT table_index = correlated_columns[0]->binding().table_idx;
    column_names.emplace_back(correlated_columns[0]->column_name());
    column_types.emplace_back(correlated_columns[0]->Type());
    column_ids.emplace_back(correlated_columns[0]->binding().column_idx);
    for(SizeT idx = 1; idx < column_count; ++ idx) {
        PlannerAssert(correlated_columns[idx]->binding().table_idx == table_index,
                      "Correlated column are from different table.");
        column_names.emplace_back(correlated_columns[idx]->column_name());
        column_types.emplace_back(correlated_columns[idx]->Type());
        column_ids.emplace_back(correlated_columns[idx]->binding().column_idx);
    }

    if(!bind_context_ptr_->binding_by_name_.contains(correlated_columns[0]->table_name())) {
        PlannerError(fmt::format("Can't find table: {} in binding context.", correlated_columns[0]->table_name()));
    }

    const SharedPtr<Binding>& table_binding = bind_context_ptr_->binding_by_name_[correlated_columns[0]->table_name()];

    SharedPtr<TableScanFunction> scan_function = TableScanFunction::Make("seq_scan");

    SharedPtr<LogicalTableScan> logical_table_scan = MakeShared<LogicalTableScan>(
            bind_context_ptr_->GetNewLogicalNodeId(),
            table_binding->table_ptr_,
            scan_function,
            table_binding->table_name_,
            table_index,
            column_ids,
            column_names,
            column_types);

    // Generate cross product
    u64 cross_product_table_index = bind_context_ptr_->GenerateTableIndex();
    String alias = "cross_product" + std::to_string(cross_product_table_index);
    SharedPtr<LogicalCrossProduct> cross_product_node = MakeShared<LogicalCrossProduct>(
            bind_context_ptr_->GetNewLogicalNodeId(),
            alias,
            cross_product_table_index,
            subquery_plan,
            logical_table_scan);

    this->parent_table_index_ = cross_product_table_index;
    this->right_offset_ = subquery_plan->GetOutputNames()->size();
    this->left_offset_ = 0;

    return cross_product_node;
}

}
