//
// Created by jinhai on 23-10-1.
//

#include "physical_merge_knn.h"
#include "executor/operator_state.h"
#include "storage/data_block.h"

namespace infinity {

void PhysicalMergeKnn::Init() {}

void PhysicalMergeKnn::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    if (input_state->Complete()) {
        LOG_TRACE("PhysicalMergeKnn::Execute complete")
    }
    auto merge_knn_input = static_cast<MergeKnnInputState *>(input_state);
    auto merge_knn_output = static_cast<MergeKnnOutputState *>(output_state);

    output_state->data_block_->AppendWith(input_state->input_data_block_);
    output_state->SetComplete();
}

void PhysicalMergeKnn::Execute(QueryContext *query_context) {}

} // namespace infinity
