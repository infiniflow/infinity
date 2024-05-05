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

// #include <fstream>
#include <string>

module physical_command;

import stl;
import query_context;
import operator_state;

import profiler;
import local_file_system;
import file_writer;
import table_def;
import data_table;
import options;
import third_party;
import defer_op;
import config;
import status;
import infinity_exception;
import compact_segments_task;

namespace infinity {

void PhysicalCommand::Init() {}

bool PhysicalCommand::Execute(QueryContext *query_context, OperatorState *operator_state) {
    DeferFn defer_fn([&]() { operator_state->SetComplete(); });
    switch (command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_command = (UseCmd *)(command_info_.get());
            query_context->set_current_schema(use_command->db_name());
            break;
        }
        case CommandType::kSet: {
            SetCmd *set_command = (SetCmd *)(command_info_.get());
            if (set_command->var_name() == enable_profiling_name) {
                if (set_command->value_type() != SetVarType::kBool) {
                    RecoverableError(Status::DataTypeMismatch("Boolean", set_command->value_type_str()));
                }
                query_context->current_session()->SessionVariables()->enable_profile_ = set_command->value_bool();
                return true;
            }

            if (set_command->var_name() == profile_history_capacity_name) {
                if (set_command->value_type() != SetVarType::kInteger) {
                    RecoverableError(Status::DataTypeMismatch("Integer", set_command->value_type_str()));
                }
                query_context->current_session()->SessionVariables()->profile_record_capacity_ = set_command->value_int();
                return true;
            }

            if (set_command->var_name() == log_level) {
                if (set_command->value_type() != SetVarType::kString) {
                    RecoverableError(Status::DataTypeMismatch("String", set_command->value_type_str()));
                }

                if (set_command->scope() != SetScope::kGlobal) {
                    RecoverableError(Status::SyntaxError(fmt::format("log_level is a global config parameter.", set_command->var_name())));
                }

                if (set_command->value_str() == "trace") {
                    SetLogLevel(LogLevel::kTrace);
                    return true;
                }

                if (set_command->value_str() == "debug") {
                    SetLogLevel(LogLevel::kDebug);
                    return true;
                }

                if (set_command->value_str() == "info") {
                    SetLogLevel(LogLevel::kInfo);
                    return true;
                }

                if (set_command->value_str() == "warning") {
                    SetLogLevel(LogLevel::kWarning);
                    return true;
                }

                if (set_command->value_str() == "error") {
                    SetLogLevel(LogLevel::kError);
                    return true;
                }

                if (set_command->value_str() == "critical") {
                    SetLogLevel(LogLevel::kCritical);
                    return true;
                }

                RecoverableError(Status::SetInvalidVarValue("log level", "trace, debug, info, warning, error, critical"));
                return true;
            }

            if (set_command->var_name() == worker_cpu_limit) {
                if (set_command->value_type() != SetVarType::kInteger) {
                    RecoverableError(Status::DataTypeMismatch("Integer", set_command->value_type_str()));
                }

                if (set_command->scope() != SetScope::kGlobal) {
                    RecoverableError(Status::SyntaxError(fmt::format("cpu_count is a global config parameter.", set_command->var_name())));
                }

                RecoverableError(Status::ReadOnlySysVar(set_command->var_name()));
                return true;
            }

            { UnrecoverableError(fmt::format("Unknown command: {}", set_command->var_name())); }
            break;
        }
        case CommandType::kExport: {
            ExportCmd *export_command = (ExportCmd *)(command_info_.get());
            auto profiler_record = query_context->current_session()->GetProfilerRecord(export_command->file_no());
            if (profiler_record == nullptr) {
                RecoverableError(Status::DataNotExist(fmt::format("The record does not exist: {}", export_command->file_no())));
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
        case CommandType::kCompactTable: {
            auto *txn = query_context->GetTxn();
            auto compact_task = CompactSegmentsTask::MakeTaskWithWholeTable(table_entry_, txn); // copy the table ref shared_ptr here
            compact_task->Execute();
            break;
        }
        default: {
            UnrecoverableError("Invalid command type.");
        }
    }
    return true;
}
} // namespace infinity