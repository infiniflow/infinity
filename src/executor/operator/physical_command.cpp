
module;

import stl;
import query_context;
import operator_state;
import parser;
import table_def;
import data_table;

import infinity_exception;

module physical_command;

namespace infinity {

void PhysicalCommand::Init() {}

void PhysicalCommand::Execute(QueryContext *query_context) {
    Error<NotImplementException>("Deprecated execute function should not be called.");
}

void PhysicalCommand::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    switch(command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_command = (UseCmd*)(command_info_.get());
            query_context->set_current_schema(use_command->db_name());
            output_state->SetComplete();
            break;
        }
        case CommandType::kCheckTable: {
            output_state->SetComplete();
            break;
        }
        case CommandType::kInvalid: {
            Error<ExecutorException>("Invalid command type.");
        }
    }
}
} // namespace infinity