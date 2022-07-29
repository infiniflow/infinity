//
// Created by JinHai on 2022/7/26.
//

#include "physical_planner.h"
#include "planner/operator/logical_create_table.h"
#include "planner/operator/logical_drop_table.h"

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
#include "executor/operator/physical_top.h"
#include "executor/operator/physical_union_all.h"
#include "executor/operator/physical_update.h"
#include "executor/operator/physical_prepared_plan.h"
#include "executor/operator/physical_dummy_operator.h"


namespace infinity {

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildPhysicalOperator(const std::shared_ptr<LogicalOperator>& logical_operator) const {

    switch(logical_operator->operator_type()) {
        // DDL
        case LogicalOperatorType::kCreateTable: return BuildCreateTable(logical_operator);
        case LogicalOperatorType::kDropTable: return BuildDropTable(logical_operator);
        case LogicalOperatorType::kCreateView: return BuildCreateView(logical_operator);
        case LogicalOperatorType::kDropView: return BuildDropView(logical_operator);
        case LogicalOperatorType::kAlter: return BuildAlter(logical_operator);

        // DML
        case LogicalOperatorType::kInsert: return BuildInsert(logical_operator);
        case LogicalOperatorType::kDelete: return BuildDelete(logical_operator);
        case LogicalOperatorType::kUpdate: return BuildUpdate(logical_operator);
        case LogicalOperatorType::kImport: return BuildImport(logical_operator);
        case LogicalOperatorType::kExport: return BuildExcept(logical_operator);

        // SELECT
        case LogicalOperatorType::kAggregate: return BuildAggregate(logical_operator);
        case LogicalOperatorType::kJoin: return BuildJoin(logical_operator);
        case LogicalOperatorType::kSort: return BuildSort(logical_operator);
        case LogicalOperatorType::kLimit: return BuildLimit(logical_operator);
        case LogicalOperatorType::kPredicate: return BuildPredicate(logical_operator);
        case LogicalOperatorType::kProjection: return BuildProjection(logical_operator);
        case LogicalOperatorType::kUnion: return BuildUnion(logical_operator);
        case LogicalOperatorType::kExcept: return BuildExcept(logical_operator);
        case LogicalOperatorType::kIntersect: return BuildIntersect(logical_operator);
        default:
            break;
    }

    return std::make_shared<PhysicalDummyOperator>(std::numeric_limits<uint64_t>::max());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildCreateTable(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    std::shared_ptr<LogicalCreateTable> logical_create_table =
            std::static_pointer_cast<LogicalCreateTable>(logical_operator);
    return std::make_shared<PhysicalCreateTable>(
            logical_create_table->schema_name(),
            logical_create_table->table_definitions(),
            logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildPreparedPlan(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalPreparedPlan>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildCreateView(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalCreateView>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDropTable(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    std::shared_ptr<LogicalDropTable> logical_drop_table =
            std::static_pointer_cast<LogicalDropTable>(logical_operator);
    return std::make_shared<PhysicalDropTable>(
            logical_drop_table->schema_name(),
            logical_drop_table->table_name(),
            logical_drop_table->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDropView(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalDropView>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildInsert(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalInsert>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDelete(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalDelete>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildUpdate(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalUpdate>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildImport(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalImport>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildExport(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalExport>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildAlter(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalAlter>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildAggregate(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalAggregate>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildJoin(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalHashJoin>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildSort(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalSort>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildLimit(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalLimit>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildProjection(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalProject>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildPredicate(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalFilter>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildIntersect(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalHashJoin>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildUnion(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalUnionAll>(logical_operator->node_id());
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildExcept(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::make_shared<PhysicalHashJoin>(logical_operator->node_id());
}





}