// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

void PhysicalCommand::Execute(QueryContext *query_context) { Error<NotImplementException>("Deprecated execute function should not be called."); }

void PhysicalCommand::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    switch (command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_command = (UseCmd *)(command_info_.get());
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