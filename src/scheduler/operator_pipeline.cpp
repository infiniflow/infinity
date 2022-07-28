//
// Created by JinHai on 2022/7/28.
//

#include "operator_pipeline.h"
#include "executor/physical_operator.h"

#include <utility>

namespace infinity {

std::shared_ptr<OperatorPipeline> OperatorPipeline::Create(const std::shared_ptr<PhysicalOperator>& op) {
    std::shared_ptr<OperatorPipeline> root_pipeline = op->GenerateOperatorPipeline();

    if(op->left()) {
        std::shared_ptr<OperatorPipeline> left_root = OperatorPipeline::Create(op->left());
        left_root->SetPredecessorOf(root_pipeline);
    }

    if(op->right()) {
        std::shared_ptr<OperatorPipeline> right_root = OperatorPipeline::Create(op->right());
        right_root->SetPredecessorOf(root_pipeline);
    }
    return root_pipeline;
}

OperatorPipeline::OperatorPipeline(std::shared_ptr<PhysicalOperator> op)
    : Pipeline(op->operator_id()), operator_(std::move(op)) {}

void
OperatorPipeline::OnExecute() {
    operator_-
}


}

