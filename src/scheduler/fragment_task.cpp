//
// Created by jinhai on 23-9-8.
//

#include "fragment_task.h"
#include "fragment_context.h"

namespace infinity {

void
FragmentTask::OnExecute(i64 worker_id) {
    LOG_TRACE("Execute fragment task on {}", worker_id);
    FragmentContext* fragment_context = (FragmentContext*)fragment_context_;
    switch(fragment_context->ContextType()) {

        case FragmentType::kGlobalMaterialized: {
            
            break;
        }
        case FragmentType::kLocalMaterialized:
            break;
        case FragmentType::kStreamFragment:
            break;
    }

    Vector<PhysicalOperator*>& operator_refs = fragment_context->GetOperators();
    for(const auto& op_ref: operator_refs) {
        op_ref->Execute(fragment_context->query_context(), input_state_ptr_, output_state_ptr_);
    }

    fragment_context->Complete();
}

}