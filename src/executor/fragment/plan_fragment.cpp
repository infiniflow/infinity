//
// Created by jinhai on 23-4-27.
//

#include "executor/fragment/plan_fragment.h"
#include "src/scheduler/fragment_context.h"

namespace infinity{

Vector<UniquePtr<FragmentTask>>&
PlanFragment::CreateTasks(QueryContext* query_context) {
    context_ = FragmentContext::MakeFragmentContext(query_context, this);
    return context_->Tasks();
}

SharedPtr<Vector<String>>
PlanFragment::ToString(){
    auto result = std::make_shared<Vector<String>>();
    if(source_ != nullptr) {
        result->push_back(source_->GetName());
    }
    for(auto& op: operators_){
        result->push_back(op->GetName());
    }
    if (sink_ != nullptr) {
        result->push_back(sink_->GetName());
    }
    return result;
}

SharedPtr<Table>
PlanFragment::GetResult() {
    return context_->GetResult();
}

}
