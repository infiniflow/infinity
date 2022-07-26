//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include "physical_operator.h"
#include "planner/logical_operator.h"

namespace infinity {

class PhysicalPlanner {
public:
    std::shared_ptr<PhysicalOperator> BuildPhysicalOperator(const std::shared_ptr<LogicalOperator>& logical_operator) const;

private:
    // Create operator
    std::shared_ptr<PhysicalOperator> BuildCreateTable(const std::shared_ptr<LogicalOperator>& logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildPreparedPlan(const std::shared_ptr<LogicalOperator>& logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildCreateView(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Drop operator
    std::shared_ptr<PhysicalOperator> BuildDropTable(const std::shared_ptr<LogicalOperator>& logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildDropView(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Insert operator
    std::shared_ptr<PhysicalOperator> BuildInsert(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Delete operator
    std::shared_ptr<PhysicalOperator> BuildDelete(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Update operator
    std::shared_ptr<PhysicalOperator> BuildUpdate(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Import operator
    std::shared_ptr<PhysicalOperator> BuildImport(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Export operator
    std::shared_ptr<PhysicalOperator> BuildExport(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Alter operator
    std::shared_ptr<PhysicalOperator> BuildAlter(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    // Select operator
    std::shared_ptr<PhysicalOperator> BuildAggregate(const std::shared_ptr<LogicalOperator>& logical_operator) const;

    std::shared_ptr<PhysicalOperator> BuildJoin(const std::shared_ptr<LogicalOperator> &logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildSort(const std::shared_ptr<LogicalOperator> &logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildLimit(const std::shared_ptr<LogicalOperator> &logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildProjection(const std::shared_ptr<LogicalOperator> &logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildPredicate(const std::shared_ptr<LogicalOperator> &logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildIntersect(const std::shared_ptr<LogicalOperator> &logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildUnion(const std::shared_ptr<LogicalOperator> &logical_operator) const;
    std::shared_ptr<PhysicalOperator> BuildExcept(const std::shared_ptr<LogicalOperator> &logical_operator) const;
};


}


