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

#include <fstream>
import stl;
import query_context;
import operator_state;
import parser;
import profiler;
import local_file_system;
import file_writer;
import table_def;
import data_table;
import options;
import third_party;

import infinity_exception;

module physical_command;

namespace infinity {

void PhysicalCommand::Init() {}

void PhysicalCommand::Execute(QueryContext *query_context, OperatorState *operator_state) {
    operator_state->SetComplete();
    switch (command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_command = (UseCmd *)(command_info_.get());
            query_context->set_current_schema(use_command->db_name());
            break;
        }
        case CommandType::kSet: {
            SetCmd *set_command = (SetCmd *)(command_info_.get());
            if(IsEqual(set_command->var_name(), enable_profiling_name)) {
                if(set_command->value_type() != SetVarType::kBool) {
                    Error<ExecutorException>(Format("Wrong value type: {}", set_command->var_name()));
                }
                query_context->current_session()->options()->enable_profiling_ = set_command->value_bool();
            } else if(IsEqual(set_command->var_name(), profile_history_capacity_name)) {
                if(set_command->value_type() != SetVarType::kInteger) {
                    Error<ExecutorException>(Format("Wrong value type: {}", set_command->var_name()));
                }
                query_context->current_session()->options()->profile_history_capacity_ = set_command->value_int();
            } else {
                Error<ExecutorException>(Format("Unknown command: {}", set_command->var_name()));
            }
            break;
        }
        case CommandType::kExport: {
            ExportCmd *export_command = (ExportCmd *)(command_info_.get());
            auto profiler_record = query_context->current_session()->GetProfilerRecord(export_command->file_no());
            if (profiler_record == nullptr) {
                Error<ExecutorException>(Format("The record does not exist: {}", export_command->file_no()));
            }
            LocalFileSystem fs;
            FileWriter file_writer(fs, export_command->file_name(), 128);

            auto json = QueryProfiler::Serialize(profiler_record).dump();
            file_writer.Write(json.c_str(), json.size());
            file_writer.Flush();
            break;
        }
        case CommandType::kCheckTable: {

            break;
        }
        case CommandType::kInvalid: {
            Error<ExecutorException>("Invalid command type.");
        }
    }

}
} // namespace infinity