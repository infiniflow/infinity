//
// Created by jinhai on 23-10-1.
//

module;

import stl;
import txn;
import query_context;
import parser;
import physical_operator_type;
import operator_state;
import logger;

module physical_merge_knn;

namespace infinity {

void PhysicalMergeKnn::Init() {}

void PhysicalMergeKnn::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    if (input_state->Complete()) {
        LOG_TRACE("PhysicalMergeKnn::Execute complete");
    }
    output_state->data_block_->AppendWith(input_state->input_data_block_);
    output_state->SetComplete();
}

void PhysicalMergeKnn::Execute(QueryContext *query_context) {}

} // namespace infinity
