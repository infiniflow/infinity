//
// Created by JinHai on 2022/7/28.
//

#include "operator_pipeline.h"

#include <utility>

namespace infinity {

OperatorPipeline::OperatorPipeline(std::shared_ptr<PhysicalOperator> op) : operator_(std::move(op)) {

}

void
OperatorPipeline::OnExecute() {

}



}

