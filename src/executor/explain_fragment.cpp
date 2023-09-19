//
// Created by jinhai on 23-9-9.
//

#include "explain_fragment.h"
#include "explain_physical_plan.h"

namespace infinity {

void
ExplainFragment::Explain(PlanFragment* fragment_ptr,
                         SharedPtr<Vector<SharedPtr<String>>>& result,
                         i64 intent_size) {
    {
        String fragment_header;
        if (intent_size != 0) {
            fragment_header = String(intent_size - 2, ' ') + "-> FRAGMENT ";
        } else {
            fragment_header = "FRAGMENT ";
        }

        fragment_header += "(" + std::to_string(fragment_ptr->FragmentID()) + ")";
        result->emplace_back(MakeShared<String>(fragment_header));
    }

    intent_size += 2;

    if(fragment_ptr->GetSinkNode()) {
        ExplainPhysicalPlan::Explain(fragment_ptr->GetSinkNode(), result, intent_size);
    }

    Vector<PhysicalOperator*>& fragment_operators = fragment_ptr->GetOperators();
    for (auto & fragment_operator : fragment_operators) {
        ExplainPhysicalPlan::Explain(fragment_operator, result, true, intent_size);
    }

    if(fragment_ptr->GetSourceNode()) {
        ExplainPhysicalPlan::Explain(fragment_ptr->GetSourceNode(), result, intent_size);
    }

    // NOTE: recursive call this function to explain child fragment
    if (!fragment_ptr->Children().empty()){
        for (auto& child : fragment_ptr->Children()) {
            ExplainFragment::Explain(child.get(), result);
        }
    }
}

}
