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
        default: {
            NotImplementError("Not support source state type");
        }
    }
}

}
