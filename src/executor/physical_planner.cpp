//
// Created by JinHai on 2022/7/26.
//

#include "physical_planner.h"
#include "planner/node/logical_create_table.h"
#include "planner/node/logical_drop_table.h"
#include "planner/node/logical_insert.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_sort.h"
#include "planner/node/logical_table_scan.h"
#include "planner/node/logical_dummy_scan.h"
#include "planner/node/logical_filter.h"
#include "planner/node/logical_limit.h"
#include "planner/node/logical_aggregate.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_explain.h"
#include "planner/node/logical_create_schema.h"
#include "planner/node/logical_drop_schema.h"
#include "planner/node/logical_create_collection.h"
#include "planner/node/logical_drop_collection.h"
#include "planner/node/logical_join.h"
#include "planner/node/logical_create_view.h"
#include "planner/node/logical_drop_view.h"
#include "planner/node/logical_flush.h"
#include "planner/node/logical_import.h"
#include "planner/node/logical_export.h"

#include "executor/operator/physcial_drop_view.h"
#include "executor/operator/physical_aggregate.h"
#include "executor/operator/physical_alter.h"
#include "executor/operator/physical_create_table.h"
#include "executor/operator/physical_create_collection.h"
#include "executor/operator/physical_create_view.h"
#include "executor/operator/physical_delete.h"
#include "executor/operator/physical_drop_table.h"
#include "executor/operator/physical_drop_collection.h"
#include "executor/operator/physical_export.h"
#include "executor/operator/physical_filter.h"
#include "executor/operator/physical_hash_join.h"
#include "executor/operator/physical_import.h"
#include "executor/operator/physical_index_join.h"
#include "executor/operator/physical_index_scan.h"
#include "executor/operator/physical_insert.h"
#include "executor/operator/physical_nested_loop_join.h"
#include "executor/operator/physical_cross_product.h"
#include "executor/operator/physical_project.h"
#include "executor/operator/physical_sort.h"
#include "executor/operator/physical_limit.h"
#include "executor/operator/physical_table_scan.h"
#include "executor/operator/physical_show.h"
#include "executor/operator/physical_top.h"
#include "executor/operator/physical_union_all.h"
#include "executor/operator/physical_update.h"
#include "executor/operator/physical_prepared_plan.h"
#include "executor/operator/physical_dummy_operator.h"
#include "executor/operator/physical_dummy_scan.h"
#include "executor/operator/physical_flush.h"
#include "executor/operator/physical_explain.h"
#include "executor/operator/physical_drop_schema.h"
#include "executor/operator/physical_create_schema.h"
#include "explain_physical_plan.h"

#include <limits>

namespace infinity {

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildPhysicalOperator(const SharedPtr<LogicalNode>& logical_operator) const {

    SharedPtr<PhysicalOperator> result;
    switch(logical_operator->operator_type()) {
        // DDL
        case LogicalNodeType::kCreateTable: {
            result = BuildCreateTable(logical_operator);
            break;
        }
        case LogicalNodeType::kCreateCollection: {
            result = BuildCreateCollection(logical_operator);
            break;
        }
        case LogicalNodeType::kCreateSchema: {
            result = BuildCreateSchema(logical_operator);
            break;
        }
        case LogicalNodeType::kDropTable: {
            result = BuildDropTable(logical_operator);
            break;
        }
        case LogicalNodeType::kDropCollection: {
            result = BuildDropCollection(logical_operator);
            break;
        }
        case LogicalNodeType::kDropSchema: {
            result = BuildDropSchema(logical_operator);
            break;
        }
        case LogicalNodeType::kCreateView: {
            result = BuildCreateView(logical_operator);
            break;
        }
        case LogicalNodeType::kDropView: {
            result = BuildDropView(logical_operator);
            break;
        }
        case LogicalNodeType::kAlter: {
            result = BuildAlter(logical_operator);
            break;
        }

        // DML
        case LogicalNodeType::kInsert: {
            result = BuildInsert(logical_operator);
            break;
        }
        case LogicalNodeType::kDelete: {
            result = BuildDelete(logical_operator);
            break;
        }
        case LogicalNodeType::kUpdate: {
            result = BuildUpdate(logical_operator);
            break;
        }
        case LogicalNodeType::kImport: {
            result = BuildImport(logical_operator);
            break;
        }
        case LogicalNodeType::kExport: {
            result = BuildExport(logical_operator);
            break;
        }

        // Scan
        case LogicalNodeType::kShow: {
            result = BuildShow(logical_operator);
            break;
        }
        case LogicalNodeType::kTableScan: {
            result = BuildTableScan(logical_operator);
            break;
        }
        case LogicalNodeType::kViewScan: {
            break;
        }
        case LogicalNodeType::kDummyScan: {
            result = BuildDummyScan(logical_operator);
            break;
        }

        // SELECT
        case LogicalNodeType::kAggregate: {
            result = BuildAggregate(logical_operator);
            break;
        }
        case LogicalNodeType::kJoin: {
            result = BuildJoin(logical_operator);
            break;
        }
        case LogicalNodeType::kCrossProduct: {
            result = BuildCrossProduct(logical_operator);
            break;
        }
        case LogicalNodeType::kSort: {
            result = BuildSort(logical_operator);
            break;
        }
        case LogicalNodeType::kLimit: {
            result = BuildLimit(logical_operator);
            break;
        }
        case LogicalNodeType::kFilter: {
            result = BuildFilter(logical_operator);
            break;
        }
        case LogicalNodeType::kProjection: {
            result = BuildProjection(logical_operator);
            break;
        }
        case LogicalNodeType::kUnion: {
            result = BuildUnion(logical_operator);
            break;
        }
        case LogicalNodeType::kExcept: {
            result = BuildExcept(logical_operator);
            break;
        }
        case LogicalNodeType::kIntersect: {
            result = BuildIntersect(logical_operator);
            break;
        }
        case LogicalNodeType::kFlush: {
            result = BuildFlush(logical_operator);
            break;
        }
        case LogicalNodeType::kExplain: {
            result = BuildExplain(logical_operator);
            break;
        }
        default: {
            PlannerError("Unknown logical node type: " + logical_operator->name());
//            result = MakeShared<PhysicalDummyOperator>(std::numeric_limits<uint64_t>::max());
        }
    }

    if(logical_operator->node_id() > query_context_ptr_->max_node_id()) {
        query_context_ptr_->set_max_node_id(logical_operator->node_id());
    }
    // Initialize the physical plan node
    result->Init();

    return result;
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildCreateTable(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateTable> logical_create_table =
            std::static_pointer_cast<LogicalCreateTable>(logical_operator);
    return MakeShared<PhysicalCreateTable>(
            logical_create_table->schema_name(),
            logical_create_table->table_definitions(),
            logical_create_table->GetOutputNames(),
            logical_create_table->GetOutputTypes(),
            logical_create_table->conflict_type(),
            logical_create_table->table_index(),
            logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildCreateCollection(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateCollection> logical_create_collection =
            std::static_pointer_cast<LogicalCreateCollection>(logical_operator);
    return MakeShared<PhysicalCreateCollection>(
            logical_create_collection->schema_name(),
            logical_create_collection->collection_name(),
            logical_create_collection->conflict_type(),
            logical_create_collection->GetOutputNames(),
            logical_create_collection->GetOutputTypes(),
            logical_create_collection->table_index(),
            logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildCreateSchema(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateSchema> logical_create_schema =
            std::static_pointer_cast<LogicalCreateSchema>(logical_operator);
    return MakeShared<PhysicalCreateSchema>(
            logical_create_schema->schema_name(),
            logical_create_schema->conflict_type(),
            logical_create_schema->GetOutputNames(),
            logical_create_schema->GetOutputTypes(),
            logical_create_schema->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildPreparedPlan(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalPreparedPlan>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildCreateView(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateView> logical_create_view =
            std::static_pointer_cast<LogicalCreateView>(logical_operator);
    return MakeShared<PhysicalCreateView>(logical_operator->node_id(),
                                          logical_create_view->names_ptr(),
                                          logical_create_view->types_ptr(),
                                          logical_create_view->create_view_info());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDropTable(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropTable> logical_drop_table =
            std::static_pointer_cast<LogicalDropTable>(logical_operator);

    return MakeShared<PhysicalDropTable>(
            logical_drop_table->schema_name(),
            logical_drop_table->table_name(),
            logical_drop_table->conflict_type(),
            logical_drop_table->GetOutputNames(),
            logical_drop_table->GetOutputTypes(),
            logical_drop_table->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDropCollection(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropCollection> logical_drop_collection =
            std::static_pointer_cast<LogicalDropCollection>(logical_operator);
    return MakeShared<PhysicalDropCollection>(
            logical_drop_collection->schema_name(),
            logical_drop_collection->collection_name(),
            logical_drop_collection->conflict_type(),
            logical_drop_collection->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDropSchema(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropSchema> logical_drop_schema =
            std::static_pointer_cast<LogicalDropSchema>(logical_operator);
    return MakeShared<PhysicalDropSchema>(
            logical_drop_schema->schema_name(),
            logical_drop_schema->conflict_type(),
            logical_drop_schema->GetOutputNames(),
            logical_drop_schema->GetOutputTypes(),
            logical_drop_schema->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDropView(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropView> logical_drop_view =
            std::static_pointer_cast<LogicalDropView>(logical_operator);
    return MakeShared<PhysicalDropView>(logical_drop_view->schema_name(),
                                        logical_drop_view->view_name(),
                                        logical_drop_view->conflict_type(),
                                        logical_drop_view->GetOutputNames(),
                                        logical_drop_view->GetOutputTypes(),
                                        logical_drop_view->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildInsert(const SharedPtr<LogicalNode> &logical_operator) const {

    SharedPtr<LogicalInsert> logical_insert_ptr = std::dynamic_pointer_cast<LogicalInsert>(logical_operator);
    return MakeShared<PhysicalInsert>(logical_operator->node_id(),
                                      logical_insert_ptr->table_collection_entry(),
                                      logical_insert_ptr->table_index(),
                                      logical_insert_ptr->value_list());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDelete(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalDelete>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildUpdate(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalUpdate>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildImport(const SharedPtr<LogicalNode> &logical_operator) const {
    LogicalImport* logical_import = (LogicalImport*)(logical_operator.get());
    return MakeShared<PhysicalImport>(logical_operator->node_id(),
                                      logical_import->table_collection_entry(),
                                      logical_import->file_path(),
                                      logical_import->header(),
                                      logical_import->delimiter(),
                                      logical_import->FileType());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildExport(const SharedPtr<LogicalNode> &logical_operator) const {
    LogicalExport* logical_export = (LogicalExport*)(logical_operator.get());
    return MakeShared<PhysicalExport>(logical_export->node_id(),
                                      logical_export->schema_name(),
                                      logical_export->table_name(),
                                      logical_export->file_path(),
                                      logical_export->header(),
                                      logical_export->delimiter(),
                                      logical_export->FileType());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildAlter(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalAlter>(logical_operator->GetOutputNames(),
                                     logical_operator->GetOutputTypes(),
                                     logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildAggregate(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    PlannerAssert(logical_operator->right_node() == nullptr,
                  "Aggregate project node shouldn't have right child.");
    SharedPtr<LogicalAggregate> logical_aggregate = std::static_pointer_cast<LogicalAggregate>(logical_operator);
    SharedPtr<PhysicalOperator> input_physical_operator{};
    if(input_logical_node != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }

    return MakeShared<PhysicalAggregate>(logical_aggregate->node_id(),
                                         input_physical_operator,
                                         logical_aggregate->groups_,
                                         logical_aggregate->groupby_index_,
                                         logical_aggregate->aggregates_,
                                         logical_aggregate->aggregate_index_);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildJoin(const SharedPtr<LogicalNode> &logical_operator) const {

    auto left_node = logical_operator->left_node();
    auto right_node = logical_operator->right_node();
    PlannerAssert(left_node != nullptr, "Join node has no left child.");
    PlannerAssert(right_node != nullptr, "Join node has no right child.");

    SharedPtr<LogicalJoin> logical_join
            = std::static_pointer_cast<LogicalJoin>(logical_operator);

    SharedPtr<PhysicalOperator> left_physical_operator{};
    SharedPtr<PhysicalOperator> right_physical_operator{};

    left_physical_operator = BuildPhysicalOperator(left_node);
    right_physical_operator = BuildPhysicalOperator(right_node);

    return MakeShared<PhysicalNestedLoopJoin>(logical_operator->node_id(),
                                              logical_join->join_type_,
                                              logical_join->conditions_,
                                              left_physical_operator,
                                              right_physical_operator);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildCrossProduct(const SharedPtr<LogicalNode> &logical_operator) const {

    auto left_node = logical_operator->left_node();
    auto right_node = logical_operator->right_node();
    PlannerAssert(left_node != nullptr, "Cross product node has no left child.");
    PlannerAssert(right_node != nullptr, "Cross product node has no right child.");

    SharedPtr<LogicalCrossProduct> logical_cross_product
                                        = std::static_pointer_cast<LogicalCrossProduct>(logical_operator);

    SharedPtr<PhysicalOperator> left_physical_operator{};
    SharedPtr<PhysicalOperator> right_physical_operator{};

    left_physical_operator = BuildPhysicalOperator(left_node);
    right_physical_operator = BuildPhysicalOperator(right_node);

    return MakeShared<PhysicalCrossProduct>(logical_operator->node_id(),
                                            left_physical_operator,
                                            right_physical_operator);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildSort(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    PlannerAssert(input_logical_node != nullptr, "Logical sort node has no input node.");
    PlannerAssert(logical_operator->right_node() == nullptr,
                  "Logical sort node shouldn't have right child.");

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    SharedPtr<LogicalSort> logical_sort = std::static_pointer_cast<LogicalSort>(logical_operator);

    return MakeShared<PhysicalSort>(logical_operator->node_id(),
                                    input_physical_operator,
                                    logical_sort->expressions_,
                                    logical_sort->order_by_types_);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildLimit(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    PlannerAssert(input_logical_node != nullptr, "Logical limit node has no input node.");
    PlannerAssert(logical_operator->right_node() == nullptr,
                  "Logical project node shouldn't have right child.");
    SharedPtr<LogicalLimit> logical_limit = std::static_pointer_cast<LogicalLimit>(logical_operator);
    SharedPtr<PhysicalOperator> input_physical_operator = BuildPhysicalOperator(input_logical_node);
    return MakeShared<PhysicalLimit>(logical_operator->node_id(),
                                     input_physical_operator,
                                     logical_limit->limit_expression_,
                                     logical_limit->offset_expression_);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildProjection(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
//    PlannerAssert(input_logical_node != nullptr, "Logical project node has no input node.");
    PlannerAssert(logical_operator->right_node() == nullptr,
                  "Logical project node shouldn't have right child.");
    SharedPtr<LogicalProject> logical_project = std::static_pointer_cast<LogicalProject>(logical_operator);
    SharedPtr<PhysicalOperator> input_physical_operator{};
    if(input_logical_node != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }
    return MakeShared<PhysicalProject>(logical_operator->node_id(),
                                       logical_project->table_index_,
                                       input_physical_operator,
                                       logical_project->expressions_);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildFilter(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    PlannerAssert(input_logical_node != nullptr, "Logical filter node has no input node.");
    PlannerAssert(logical_operator->right_node() == nullptr,
                  "Logical project node shouldn't have right child.");

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    SharedPtr<LogicalFilter> logical_filter = std::static_pointer_cast<LogicalFilter>(logical_operator);

    return MakeShared<PhysicalFilter>(logical_operator->node_id(),
                                      input_physical_operator,
                                      logical_filter->expression());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildIntersect(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalHashJoin>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildUnion(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalUnionAll>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildExcept(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalHashJoin>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildShow(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalShow> logical_show =
            std::static_pointer_cast<LogicalShow>(logical_operator);
    return MakeShared<PhysicalShow>(logical_show->node_id(),
                                    logical_show->scan_type(),
                                    logical_show->schema_name(),
                                    logical_show->object_name(),
                                    logical_show->table_index());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildTableScan(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalTableScan> logical_table_scan =
            std::static_pointer_cast<LogicalTableScan>(logical_operator);

//    HashMap<String, size_t> name2index;
//    size_t column_count = logical_table_scan->table_ptr()->ColumnCount();
//    for(size_t idx = 0; idx < column_count; ++ idx) {
//        name2index.emplace(logical_table_scan->table_ptr()->GetColumnNameById(idx), idx);
//    }
//
//    Vector<size_t> column_ids;
//    column_ids.reserve(logical_table_scan->column_names_.size());
//    for(const auto& column_name: logical_table_scan->column_names_) {
//        if(name2index.contains(column_name)) {
//            column_ids.emplace_back(name2index[column_name]);
//        } else {
//            PlannerError("Unknown column table_name: " + column_name + " when building physical plan.");
//        }
//    }

    return MakeShared<PhysicalTableScan>(logical_operator->node_id(),
                                         logical_table_scan->base_table_ref_);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildViewScan(const SharedPtr<LogicalNode> &logical_operator) const {
    NotImplementError("BuildViewScan");
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDummyScan(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDummyScan> logical_show =
            std::static_pointer_cast<LogicalDummyScan>(logical_operator);
    return MakeShared<PhysicalDummyScan>(logical_show->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildFlush(const SharedPtr<LogicalNode>& logical_operator) const {
    LogicalFlush* logical_flush = (LogicalFlush*)(logical_operator.get());
    return MakeShared<PhysicalFlush>(logical_flush->flush_type(), logical_flush->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildExplain(const SharedPtr<LogicalNode>& logical_operator) const {

    auto input_logical_node = logical_operator->left_node();
    SharedPtr<PhysicalOperator> input_physical_operator{nullptr};
    if(input_logical_node != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }

    SharedPtr<LogicalExplain> logical_explain =
            std::static_pointer_cast<LogicalExplain>(logical_operator);

    SharedPtr<PhysicalExplain> explain_node{nullptr};
    switch (logical_explain->explain_type()) {
        case ExplainType::kAnalyze: {
            NotImplementError("Explain analyze");
            break;
        }
        case ExplainType::kAst:
        case ExplainType::kUnOpt:
        case ExplainType::kOpt: {
            explain_node = MakeShared<PhysicalExplain>(logical_explain->node_id(),
                                                       logical_explain->explain_type(),
                                                       logical_explain->TextArray(),
                                                       nullptr);
            break;
        }
        case ExplainType::kPhysical: {
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            ExplainPhysicalPlan::Explain(input_physical_operator.get(), texts_ptr);
            explain_node = MakeShared<PhysicalExplain>(logical_explain->node_id(),
                                                       logical_explain->explain_type(),
                                                       texts_ptr,
                                                       nullptr);
            break;
        }
        case ExplainType::kPipeline: {
            explain_node = MakeShared<PhysicalExplain>(logical_explain->node_id(),
                                                       logical_explain->explain_type(),
                                                       nullptr,
                                                       input_physical_operator);
            break;
        }
    }

    return explain_node;
}

}
