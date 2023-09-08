//
// Created by jinhai on 23-4-27.
//

#include "executor/pipeline/plan_fragment.h"

namespace infinity{

SharedPtr<Vector<String>>
PlanFragment::ToString(){
    auto result = std::make_shared<Vector<String>>();
    std::cout<<"---- fragment ----"<<std::endl;
    result->push_back(source_->GetName());
    std::cout<<"source: "<<source_->GetName()<<std::endl;
    for(auto& op: operators_){
        result->push_back(op->GetName());
        std::cout<<"op: "<<op->GetName()<<std::endl;
    }
    if (sink_ != nullptr) {
        result->push_back(sink_->GetName());
        std::cout<<"sink: "<<sink_->GetName()<<std::endl;
    }
    return result;
}
}
