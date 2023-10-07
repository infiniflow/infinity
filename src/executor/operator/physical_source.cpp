//
// Created by jinhai on 23-4-25.
//

#include "physical_source.h"

namespace infinity {

void
PhysicalSource::Init() {

}

void
PhysicalSource::Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) {

}

void
PhysicalSource::Execute(QueryContext* query_context) {

}

void
PhysicalSource::Execute(QueryContext* query_context, SourceState* source_state) {
    switch(source_state->state_type_) {
        case SourceStateType::kKnnScan:
        case SourceStateType::kTableScan: {
//            auto* table_scan_source_state = static_cast<TableScanSourceState*>(source_state);
//            auto* table_scan_input_state = static_cast<TableScanInputState*>(table_scan_source_state->next_input_state_);
            break;
        }
        case SourceStateType::kEmpty: {
            break;
        }
        case SourceStateType::kQueue: {
            QueueSourceState* queue_source_state = static_cast<QueueSourceState*>(source_state);
            queue_source_state->source_queue_.Dequeue(queue_source_state->current_fragment_data_);
            if(source_state->next_input_state_->total_data_count_ == 0) {
                source_state->next_input_state_->total_data_count_ = queue_source_state->current_fragment_data_->data_count_;
            }
            source_state->next_input_state_->input_data_block_ = queue_source_state->current_fragment_data_->data_block_.get();
            ++ source_state->next_input_state_->received_data_count_;
            break;
        }
        default: {
            NotImplementError("Not support source state type");
        }
    }
}

bool
PhysicalSource::ReadyToExec(SourceState* source_state) {
    if(source_state->state_type_ == SourceStateType::kQueue) {
        QueueSourceState* queue_source_state = static_cast<QueueSourceState*>(source_state);
        return queue_source_state->source_queue_.ApproxSize() > 0;
    }
    return true;
}

}
