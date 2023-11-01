//
// Created by jinhai on 23-9-9.
//

module;

import std;
import stl;
import plan_fragment;
import explain_physical_plan;
import physical_operator;

module explain_fragment;

namespace infinity {

void ExplainFragment::Explain(PlanFragment *fragment_ptr, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String fragment_header;
        if (intent_size != 0) {
            fragment_header = String(intent_size - 2, ' ') + "-> FRAGMENT ";
        } else {
            fragment_header = "FRAGMENT ";
        }

        fragment_header += "(" + ToStr(fragment_ptr->FragmentID()) + ")";
        result->emplace_back(MakeShared<String>(fragment_header));
    }

    if (fragment_ptr->GetSinkNode()) {
        ExplainPhysicalPlan::Explain(fragment_ptr->GetSinkNode(), result, false, intent_size);
    }

    Vector<PhysicalOperator *> &fragment_operators = fragment_ptr->GetOperators();
    for (auto &fragment_operator : fragment_operators) {
        ExplainPhysicalPlan::Explain(fragment_operator, result, false, intent_size);
    }

    if (fragment_ptr->GetSourceNode()) {
        ExplainPhysicalPlan::Explain(fragment_ptr->GetSourceNode(), result, false, intent_size);
    }

    intent_size += 2;

    // NOTE: recursive call this function to explain child fragment
    if (!fragment_ptr->Children().empty()) {
        for (auto &child : fragment_ptr->Children()) {
            intent_size += 2;
            ExplainFragment::Explain(child.get(), result, intent_size);
        }
    }
}

} // namespace infinity
