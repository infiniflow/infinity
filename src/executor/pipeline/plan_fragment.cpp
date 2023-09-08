//
// Created by jinhai on 23-4-27.
//

#include "executor/pipeline/plan_fragment.h"

namespace infinity{

SharedPtr<Vector<String>>
PlanFragment::ToString(){
    auto result = std::make_shared<Vector<String>>();
    result->push_back(source_->GetName());
    for(auto& op: operators_){
        result->push_back(op->GetName());
    }
    if (sink_ != nullptr) {
        result->push_back(sink_->GetName());
    }
    return result;
}
}
