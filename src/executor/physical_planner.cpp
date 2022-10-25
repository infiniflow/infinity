//
// Created by JinHai on 2022/7/26.
//

#include "physical_planner.h"
#include "planner/node/logical_create_table.h"
#include "planner/node/logical_drop_table.h"
#include "planner/node/logical_insert.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_table_scan.h"
#include "planner/node/logical_dummy_scan.h"

#include "executor/operator/physcial_drop_view.h"
#include "executor/operator/physical_aggregate.h"
#include "executor/operator/physical_alter.h"
#include "executor/operator/physical_create_table.h"
#include "executor/operator/physical_create_view.h"
#include "executor/operator/physical_delete.h"
#include "executor/operator/physical_drop_table.h"
#include "executor/operator/physical_export.h"
#include "executor/operator/physical_filter.h"
#include "executor/operator/physical_hash_join.h"
#include "executor/operator/physical_import.h"
#include "executor/operator/physical_index_join.h"
#include "executor/operator/physical_index_scan.h"
#include "executor/operator/physical_insert.h"
#include "executor/operator/physical_nested_loop_join.h"
#include "executor/operator/physical_product.h"
#include "executor/operator/physical_project.h"
#include "executor/operator/physical_sort.h"
#include "executor/operator/physical_limit.h"
#include "executor/operator/physical_table_scan.h"
#include "executor/operator/physical_chunk_scan.h"
#include "executor/operator/physical_top.h"
#include "executor/operator/physical_union_all.h"
#include "executor/operator/physical_update.h"
#include "executor/operator/physical_prepared_plan.h"
#include "executor/operator/physical_dummy_operator.h"
#include "executor/operator/physical_dummy_scan.h"

#include <limits>

namespace infinity {

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildPhysicalOperator(const std::shared_ptr<LogicalNode>& logical_operator) const {

    switch(logical_operator->operator_type()) {
        // DDL
        case LogicalNodeType::kCreateTable: return BuildCreateTable(logical_operator);
        case LogicalNodeType::kDropTable: return BuildDropTable(logical_operator);
        case LogicalNodeType::kCreateView: return BuildCreateView(logical_operator);
        case LogicalNodeType::kDropView: return BuildDropView(logical_operator);
        case LogicalNodeType::kAlter: return BuildAlter(logical_operator);

        // DML
        case LogicalNodeType::kInsert: return BuildInsert(logical_operator);
        case LogicalNodeType::kDelete: return BuildDelete(logical_operator);
        case LogicalNodeType::kUpdate: return BuildUpdate(logical_operator);
        case LogicalNodeType::kImport: return BuildImport(logical_operator);
        case LogicalNodeType::kExport: return BuildExcept(logical_operator);

        // Scan
        case LogicalNodeType::kChunkScan: return BuildChunkScan(logical_operator);
        case LogicalNodeType::kTableScan: return BuildTableScan(logical_operator);
        case LogicalNodeType::kDummyScan: return BuildDummyScan(logical_operator);

        // SELECT
        case LogicalNodeType::kAggregate: return BuildAggregate(logical_operator);
        case LogicalNodeType::kJoin: return BuildJoin(logical_operator);
        case LogicalNodeType::kSort: return BuildSort(logical_operator);
        case LogicalNodeType::kLimit: return BuildLimit(logical_operator);
        case LogicalNodeType::kFilter: return BuildFilter(logical_operator);
        case LogicalNodeType::kProjection: return BuildProjection(logical_operator);
        case LogicalNodeType::kUnion: return BuildUnion(logical_operator);
        case LogicalNodeType::kExcept: return BuildExcept(logical_operator);
        case LogicalNodeType::kIntersect: return BuildIntersect(logical_operator);
        default:
            break;
    }
    return std::make_shared<PhysicalDummyOperator>(std::numeric_limits<uint64_t>::max());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildCreateTable(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalCreateTable> logical_create_table =
            std::static_pointer_cast<LogicalCreateTable>(logical_operator);
    return std::make_shared<PhysicalCreateTable>(
            logical_create_table->schema_name(),
            logical_create_table->table_definitions(),
            logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildPreparedPlan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalPreparedPlan>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildCreateView(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalCreateView>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDropTable(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDropTable> logical_drop_table =
            std::static_pointer_cast<LogicalDropTable>(logical_operator);
    return std::make_shared<PhysicalDropTable>(
            logical_drop_table->schema_name(),
            logical_drop_table->table_name(),
            logical_drop_table->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDropView(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalDropView>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildInsert(const std::shared_ptr<LogicalNode> &logical_operator) const {

    std::shared_ptr<LogicalInsert> logical_insert_ptr = std::dynamic_pointer_cast<LogicalInsert>(logical_operator);
    return std::make_shared<PhysicalInsert>(logical_operator->node_id(), logical_insert_ptr->table_ptr(), logical_insert_ptr->value_list());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDelete(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalDelete>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildUpdate(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalUpdate>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildImport(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalImport>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildExport(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalExport>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildAlter(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalAlter>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildAggregate(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalAggregate>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildJoin(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalHashJoin>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildSort(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalSort>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildLimit(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalLimit>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildProjection(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    PlannerAssert(input_logical_node != nullptr, "Logical project node has no input node.");
    PlannerAssert(logical_operator->right_node() == nullptr,
                  "Logical project node shouldn't have right child.");

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    std::shared_ptr<LogicalProject> logical_project = std::static_pointer_cast<LogicalProject>(logical_operator);

    return std::make_shared<PhysicalProject>(logical_operator->node_id(),
                                             input_physical_operator,
                                             logical_project->expressions_);
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildFilter(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalFilter>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildIntersect(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalHashJoin>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildUnion(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalUnionAll>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildExcept(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_shared<PhysicalHashJoin>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildChunkScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalChunkScan> logical_chunk_scan =
            std::static_pointer_cast<LogicalChunkScan>(logical_operator);
    return std::make_shared<PhysicalChunkScan>(logical_chunk_scan->node_id(),
                                               logical_chunk_scan->scan_type());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildTableScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalTableScan> logical_table_scan =
            std::static_pointer_cast<LogicalTableScan>(logical_operator);

    std::unordered_map<std::string, int64_t> name2index;
    int64_t column_count = logical_table_scan->table_ptr()->table_def()->column_count();
    for(int64_t i = 0; i < column_count; ++ i) {
        name2index.emplace(logical_table_scan->table_ptr()->table_def()->columns()[i].name(), i);
    }

    std::vector<int64_t> column_ids;
    column_ids.reserve(logical_table_scan->column_names_.size());
    for(const auto& column_name: logical_table_scan->column_names_) {
        if(name2index.contains(column_name)) {
            column_ids.emplace_back(name2index[column_name]);
        } else {
            PlannerError("Unknown column name: " + column_name + " when building physical plan.");
        }
    }

    std::shared_ptr<TableScanFunctionData> table_scan_function_data_ptr
        = std::make_shared<TableScanFunctionData>(logical_table_scan->table_ptr(), column_ids);

    return std::make_shared<PhysicalTableScan>(logical_operator->node_id(),
                                               logical_table_scan->table_alias_,
                                               logical_table_scan->column_names_,
                                               logical_table_scan->column_types_,
                                               logical_table_scan->table_scan_func_ptr_,
                                               table_scan_function_data_ptr);
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDummyScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDummyScan> logical_chunk_scan =
            std::static_pointer_cast<LogicalDummyScan>(logical_operator);
    return std::make_shared<PhysicalDummyScan>(logical_chunk_scan->node_id());
}

}
