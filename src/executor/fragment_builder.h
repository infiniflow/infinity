//
// Created by jinhai on 23-4-27.
//

#include "main/query_context.h"
#include "physical_operator.h"
#include "executor/pipeline/plan_fragment.h"
#include "executor/operator/physical_table_scan.h"

#include <memory>

namespace infinity {

class FragmentBuilder {
public:
    explicit
    FragmentBuilder(QueryContext* query_context_ptr)
    : query_context_ptr_(query_context_ptr) {}

    [[nodiscard]] SharedPtr<PlanFragment>
    Build(const SharedPtr<PhysicalOperator>& phys_op) const;

private:
    [[nodiscard]] SharedPtr<PlanFragment>
    BuildAggregate(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildUnionAll(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildFilter(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildIndexScan(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildDummyScan(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildHashJoin(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildNestedLoopJoin(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildMergeJoin(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildIndexJoin(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildCrossProduct(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildLimit(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildTop(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildProject(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildSort(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildDelete(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildUpdate(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildInsert(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildImport(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildExport(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildAlter(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildCreateTable(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildCreateCollection(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildCreateSchema(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildCreateView(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildDropTable(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildDropCollection(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildDropSchema(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildDropView(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildExplain(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildPreparePlan(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildShow(const SharedPtr<PhysicalOperator>& phys_op) const;

    [[nodiscard]] SharedPtr<PlanFragment>
    BuildTableScan(const SharedPtr<PhysicalOperator>& phys_op) const;

private:
    QueryContext* query_context_ptr_{};
};

}