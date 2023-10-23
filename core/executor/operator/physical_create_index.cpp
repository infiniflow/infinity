module;

import stl;
import txn;
import query_context;
import table_def;
import table;
import parser;
import physical_operator_type;
import operator_state;

module physical_create_index;

namespace infinity {
void PhysicalCreateIndex::Init() {}

void PhysicalCreateIndex::Execute(QueryContext *query_context) {
    // TODO shenyushi -1: delete old execute function
}

void PhysicalCreateIndex::Execute(QueryContext *query_context,
                                  InputState *input_state,
                                  OutputState *output_state) {
    auto create_index_input_state =
        static_cast<CreateIndexInputState *>(input_state);
    auto create_index_output_state =
        static_cast<CreateIndexOutputState *>(output_state);

    auto* txn = query_context->GetTxn();
    auto result = txn->CreateIndex(*schema_name_, *table_name_, index_def_ptr_,
                                   conflict_type_);
    if (result.err_.get() != nullptr) {
        output_state->error_message_ = Move(result.err_);
    }
    output_state->SetComplete();
}

} // namespace infinity