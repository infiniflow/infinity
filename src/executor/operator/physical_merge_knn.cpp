//
// Created by jinhai on 23-10-1.
//

#include "physical_merge_knn.h"

namespace infinity {

void PhysicalMergeKnn::Init() {}

void PhysicalMergeKnn::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    if (input_state->Complete()) {
        LOG_TRACE("PhysicalMergeKnn::Execute complete")
    }
    output_state->SetComplete();
}

void PhysicalMergeKnn::Execute(QueryContext *query_context) {}

} // namespace infinity
