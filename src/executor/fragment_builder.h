//
// Created by jinhai on 23-4-27.
//

#include "main/query_context.h"
#include "physical_operator.h"
#include "executor/fragment/plan_fragment.h"
#include "executor/operator/physical_table_scan.h"

#include <memory>

namespace infinity {

class FragmentBuilder {
public:
    explicit
    FragmentBuilder(QueryContext* query_context_ptr)
    : query_context_ptr_(query_context_ptr) {}

    UniquePtr<PlanFragment>
    BuildFragment(PhysicalOperator* phys_op);

    void
    BuildFragments(PhysicalOperator* phys_op, PlanFragment *current_fragment_ptr);

private:

    void
    BuildExplain(PhysicalOperator* phys_op, PlanFragment* current_fragment_ptr);

    idx_t
    GetFragmentId() {
        return fragment_id_++;
    }

private:
    QueryContext* query_context_ptr_{};
    idx_t fragment_id_{};
};

}