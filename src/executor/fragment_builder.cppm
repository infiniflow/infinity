//
// Created by jinhai on 23-4-27.
//

module;

import stl;
import query_context;
import plan_fragment;
import physical_operator;

export module fragment_builder;

namespace infinity {

export class FragmentBuilder {
public:
    explicit FragmentBuilder(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {}

    UniquePtr<PlanFragment> BuildFragment(PhysicalOperator *phys_op);

    void BuildFragments(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr);

private:
    void BuildExplain(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr);

    idx_t GetFragmentId() { return fragment_id_++; }

private:
    QueryContext *query_context_ptr_{};
    idx_t fragment_id_{};
};

} // namespace infinity