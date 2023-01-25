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
#include "planner/node/logical_filter.h"

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
        case LogicalNodeType::kDropTable: {
            result = BuildDropTable(logical_operator);
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
            result = BuildExcept(logical_operator);
            break;
        }

        // Scan
        case LogicalNodeType::kChunkScan: {
            result = BuildChunkScan(logical_operator);
            break;
        }
        case LogicalNodeType::kTableScan: {
            result = BuildTableScan(logical_operator);
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
        default: {
            result = MakeShared<PhysicalDummyOperator>(std::numeric_limits<uint64_t>::max());
        }
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
            logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildPreparedPlan(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalPreparedPlan>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildCreateView(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalCreateView>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDropTable(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropTable> logical_drop_table =
            std::static_pointer_cast<LogicalDropTable>(logical_operator);
    return MakeShared<PhysicalDropTable>(
            logical_drop_table->schema_name(),
            logical_drop_table->table_name(),
            logical_drop_table->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDropView(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalDropView>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildInsert(const SharedPtr<LogicalNode> &logical_operator) const {

    SharedPtr<LogicalInsert> logical_insert_ptr = std::dynamic_pointer_cast<LogicalInsert>(logical_operator);
    return MakeShared<PhysicalInsert>(logical_operator->node_id(), logical_insert_ptr->table_ptr(), logical_insert_ptr->value_list());
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
    return MakeShared<PhysicalImport>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildExport(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalExport>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildAlter(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalAlter>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildAggregate(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalAggregate>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildJoin(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeShared<PhysicalHashJoin>(logical_operator->node_id());
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
    return MakeShared<PhysicalLimit>(logical_operator->node_id());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildProjection(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    PlannerAssert(input_logical_node != nullptr, "Logical project node has no input node.");
    PlannerAssert(logical_operator->right_node() == nullptr,
                  "Logical project node shouldn't have right child.");

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    SharedPtr<LogicalProject> logical_project = std::static_pointer_cast<LogicalProject>(logical_operator);

    return MakeShared<PhysicalProject>(logical_operator->node_id(),
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
PhysicalPlanner::BuildChunkScan(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalChunkScan> logical_chunk_scan =
            std::static_pointer_cast<LogicalChunkScan>(logical_operator);
    return MakeShared<PhysicalChunkScan>(logical_chunk_scan->node_id(),
                                               logical_chunk_scan->scan_type());
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildTableScan(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalTableScan> logical_table_scan =
            std::static_pointer_cast<LogicalTableScan>(logical_operator);

    HashMap<String, size_t> name2index;
    size_t column_count = logical_table_scan->table_ptr()->ColumnCount();
    for(size_t idx = 0; idx < column_count; ++ idx) {
        name2index.emplace(logical_table_scan->table_ptr()->GetColumnNameById(idx), idx);
    }

    Vector<size_t> column_ids;
    column_ids.reserve(logical_table_scan->column_names_.size());
    for(const auto& column_name: logical_table_scan->column_names_) {
        if(name2index.contains(column_name)) {
            column_ids.emplace_back(name2index[column_name]);
        } else {
            PlannerError("Unknown column name: " + column_name + " when building physical plan.");
        }
    }

    SharedPtr<TableScanFunctionData> table_scan_function_data_ptr
        = MakeShared<TableScanFunctionData>(logical_table_scan->table_ptr(), column_ids);

    return MakeShared<PhysicalTableScan>(logical_operator->node_id(),
                                               logical_table_scan->table_alias_,
                                               logical_table_scan->column_names_,
                                               logical_table_scan->column_types_,
                                               logical_table_scan->table_scan_func_ptr_,
                                               table_scan_function_data_ptr);
}

SharedPtr<PhysicalOperator>
PhysicalPlanner::BuildDummyScan(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDummyScan> logical_chunk_scan =
            std::static_pointer_cast<LogicalDummyScan>(logical_operator);
    return MakeShared<PhysicalDummyScan>(logical_chunk_scan->node_id());
}

}
