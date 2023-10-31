//
// Created by jinhai on 23-4-25.
//
module;

import stl;
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import data_block;
import infinity_assert;
import infinity_exception;

module physical_source;

namespace infinity {

void PhysicalSource::Init() {}

void PhysicalSource::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {}

void PhysicalSource::Execute(QueryContext *query_context) {}

void PhysicalSource::Execute(QueryContext *query_context, SourceState *source_state) {
    switch (source_state->state_type_) {
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
            QueueSourceState *queue_source_state = static_cast<QueueSourceState *>(source_state);
            queue_source_state->source_queue_.Dequeue(queue_source_state->current_fragment_data_);
            queue_source_state->SetTotalDataCount(queue_source_state->current_fragment_data_->data_count_);
            queue_source_state->PushData(queue_source_state->current_fragment_data_->data_block_.get());
            break;
        }
        default: {
            Error<NotImplementException>("Not support source state type",
                                         __FILE_NAME__,
                                         __LINE__);
        }
    }
}

bool PhysicalSource::ReadyToExec(SourceState *source_state) {

    bool result = true;
    if (source_state->state_type_ == SourceStateType::kQueue) {
        QueueSourceState *queue_source_state = static_cast<QueueSourceState *>(source_state);
        result = queue_source_state->source_queue_.ApproxSize() > 0;
    }
    return result;
}

} // namespace infinity
