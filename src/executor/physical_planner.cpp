//
// Created by JinHai on 2022/7/26.
//

#include "physical_planner.h"

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

    return std::make_shared<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildCreateTable(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildPreparedPlan(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildCreateView(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDropTable(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDropView(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildInsert(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildDelete(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildUpdate(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildImport(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildExport(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildAlter(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildAggregate(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildJoin(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildSort(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildLimit(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildProjection(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildPredicate(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildIntersect(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildUnion(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}

std::shared_ptr<PhysicalOperator>
PhysicalPlanner::BuildExcept(const std::shared_ptr<LogicalOperator> &logical_operator) const {
    return std::shared_ptr<PhysicalOperator>();
}





}