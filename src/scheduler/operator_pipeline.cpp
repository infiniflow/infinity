//
// Created by JinHai on 2022/7/28.
//

#include "operator_pipeline.h"
#include "executor/physical_operator.h"

#include <utility>

namespace infinity {

SharedPtr<OperatorPipeline> OperatorPipeline::Create(const SharedPtr<PhysicalOperator>& op) {
    SharedPtr<OperatorPipeline> root_pipeline = op->GenerateOperatorPipeline();

    if(op->left()) {
        SharedPtr<OperatorPipeline> left_root = OperatorPipeline::Create(op->left());
        left_root->SetPredecessorOf(root_pipeline);
    }

    if(op->right()) {
        SharedPtr<OperatorPipeline> right_root = OperatorPipeline::Create(op->right());
        right_root->SetPredecessorOf(root_pipeline);
    }
    return root_pipeline;
}

OperatorPipeline::OperatorPipeline(SharedPtr<PhysicalOperator> op)
    : Pipeline(op->node_id()), operator_(std::move(op)) {}

void
OperatorPipeline::OnExecute(SharedPtr<QueryContext>& query_context) {
    operator_->Execute(query_context);
}

SharedPtr<Table>
OperatorPipeline::GetResult() {
    ExecutorAssert(operator_->output() != nullptr, "No input table.");
    return operator_->output();
}

}

