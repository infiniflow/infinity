//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include "physical_operator.h"
#include "planner/logical_node.h"

namespace infinity {

class QueryContext;

class PhysicalPlanner {
public:
    explicit PhysicalPlanner(SharedPtr<QueryContext> query_context_ptr)
        : query_context_ptr_(std::move(query_context_ptr)) {}

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildPhysicalOperator(const SharedPtr<LogicalNode>& logical_operator) const;

private:
    SharedPtr<QueryContext> query_context_ptr_;

    // Create operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildCreateTable(const SharedPtr<LogicalNode>& logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildPreparedPlan(const SharedPtr<LogicalNode>& logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildCreateView(const SharedPtr<LogicalNode>& logical_operator) const;

    // Drop operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildDropTable(const SharedPtr<LogicalNode>& logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildDropView(const SharedPtr<LogicalNode>& logical_operator) const;

    // Insert operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildInsert(const SharedPtr<LogicalNode>& logical_operator) const;

    // Delete operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildDelete(const SharedPtr<LogicalNode>& logical_operator) const;

    // Update operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildUpdate(const SharedPtr<LogicalNode>& logical_operator) const;

    // Import operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildImport(const SharedPtr<LogicalNode>& logical_operator) const;

    // Export operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildExport(const SharedPtr<LogicalNode>& logical_operator) const;

    // Alter operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildAlter(const SharedPtr<LogicalNode>& logical_operator) const;

    // Select operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildAggregate(const SharedPtr<LogicalNode>& logical_operator) const;

    // Operator
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildJoin(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildCrossProduct(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildSort(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildLimit(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildProjection(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildFilter(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildIntersect(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildUnion(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildExcept(const SharedPtr<LogicalNode> &logical_operator) const;

    // Scan
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildChunkScan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildTableScan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildViewScan(const SharedPtr<LogicalNode> &logical_operator) const;
    
    [[nodiscard]] SharedPtr<PhysicalOperator>
    BuildDummyScan(const SharedPtr<LogicalNode> &logical_operator) const;
};


}


