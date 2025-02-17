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

import compilation_config;

#ifdef ENABLE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

module physical_command;

import stl;
import query_context;
import operator_state;

import profiler;
import file_writer;
import table_def;
import data_table;
import options;
import third_party;
import defer_op;
import config;
import status;
import infinity_exception;
import variables;
import logger;
import table_entry;
import txn;
import cleanup_scanner;
import infinity_context;
import periodic_trigger;
import bg_task;
import wal_manager;
import result_cache_manager;
import snapshot;

namespace infinity {

void PhysicalCommand::Init(QueryContext* query_context) {}

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
            switch (set_command->scope()) {
                case SetScope::kSession: {
                    SessionVariable session_var = VarUtil::GetSessionVarByName(set_command->var_name());
                    switch (session_var) {
                        case SessionVariable::kInvalid: {
                            Status status = Status::InvalidCommand(fmt::format("Unknown session variable: {}", set_command->var_name()));
                            RecoverableError(status);
                        }
                        default: {
                            Status status = Status::InvalidCommand(fmt::format("Session variable: {} is read-only", set_command->var_name()));
                            RecoverableError(status);
                        }
                    }
                    break;
                }
                case SetScope::kGlobal: {
                    GlobalVariable global_var = VarUtil::GetGlobalVarByName(set_command->var_name());
                    switch (global_var) {
                        case GlobalVariable::kEnableProfile: {
                            if (set_command->value_type() != SetVarType::kBool) {
                                Status status = Status::DataTypeMismatch("Boolean", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            InfinityContext::instance().storage()->catalog()->SetProfile(set_command->value_bool());
                            return true;
                        }
                        case GlobalVariable::kProfileRecordCapacity: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i32 value_int = set_command->value_int();
                            if (value_int < 0) {
                                Status status =
                                    Status::InvalidCommand(fmt::format("Try to set profile record capacity with invalid value {}", value_int));
                                RecoverableError(status);
                            }
                            query_context->storage()->catalog()->ResizeProfileHistory(value_int);
                            return true;
                        }
                        case GlobalVariable::kInvalid: {
                            Status status = Status::InvalidCommand(fmt::format("unknown global variable {}", set_command->var_name()));
                            RecoverableError(status);
                        }
                        case GlobalVariable::kJeProf: {
#ifdef ENABLE_JEMALLOC
                            // dump jeprof
                            int ret = mallctl("prof.dump", nullptr, nullptr, nullptr, 0);
                            if (ret != 0) {
                                Status status = Status::UnexpectedError(fmt::format("mallctl prof1.dump failed {}", ret));
                                RecoverableError(status);
                            }
                            return true;
#else
                            Status status = Status::InvalidCommand("jemalloc is not enabled");
                            RecoverableError(status);
#endif
                        }
                        case GlobalVariable::kFollowerNum: {
                            i64 value_int = set_command->value_int();
                            if (value_int < 0) {
                                Status status =
                                    Status::InvalidCommand(fmt::format("Attempt to set global variable: {} value as {}, which should >= 0",
                                                                       set_command->var_name(),
                                                                       value_int));
                                RecoverableError(status);
                            }
                            Status status = InfinityContext::instance().cluster_manager()->SetFollowerNumber(value_int);
                            if (!status.ok()) {
                                RecoverableError(status);
                            }
                            return true;
                        }
                        default: {
                            Status status = Status::InvalidCommand(fmt::format("Global variable: {} is read-only", set_command->var_name()));
                            RecoverableError(status);
                        }
                    }
                    break;
                }
                case SetScope::kConfig: {
                    Config *config = query_context->global_config();
                    GlobalOptionIndex config_index = config->GetOptionIndex(set_command->var_name());
                    switch (config_index) {
                        case GlobalOptionIndex::kResultCache: {
                            if (set_command->value_type() != SetVarType::kString) {
                                Status status = Status::DataTypeMismatch("String", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            String cmd = set_command->value_str();
                            ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();
                            const String &result_cache_status = config->ResultCache();

                            if (result_cache_status == "off" && cmd != "on") {
                                return true;
                            }
                            if (cmd == "on") {
                                config->SetCacheResult("on");
                                return true;
                            }
                            if (cmd == "off") {
                                cache_mgr->ClearCache();
                                config->SetCacheResult("off");
                                return true;
                            }
                            if (cmd == "suspend") {
                                config->SetCacheResult("suspend");
                                return true;
                            }
                            if (cmd == "clear") {
                                cache_mgr->ClearCache();
                                return true;
                            }
                            Status status = Status::SetInvalidVarValue("cache result", "on, off");
                            RecoverableError(status);
                            break;
                        }
                        case GlobalOptionIndex::kCacheResultCapacity: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i64 cache_num = set_command->value_int();
                            ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();
                            const String &result_cache_status = config->ResultCache();
                            if (result_cache_status == "off") {
                                Status status = Status::InvalidCommand(fmt::format("Result cache manager is off"));
                                RecoverableError(status);
                            }
                            if (cache_num < 0) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set cache result num: {}", cache_num));
                                RecoverableError(status);
                            }
                            cache_mgr->ResetCacheNumCapacity(cache_num);
                            break;
                        }
                        case GlobalOptionIndex::kLogLevel: {
                            if (set_command->value_type() != SetVarType::kString) {
                                Status status = Status::DataTypeMismatch("String", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            if (set_command->value_str() == "trace") {
                                SetLogLevel(LogLevel::kTrace);
                                config->SetLogLevel(LogLevel::kTrace);
                                return true;
                            }

                            if (set_command->value_str() == "debug") {
                                SetLogLevel(LogLevel::kDebug);
                                config->SetLogLevel(LogLevel::kDebug);
                                return true;
                            }

                            if (set_command->value_str() == "info") {
                                SetLogLevel(LogLevel::kInfo);
                                config->SetLogLevel(LogLevel::kInfo);
                                return true;
                            }

                            if (set_command->value_str() == "warning") {
                                SetLogLevel(LogLevel::kWarning);
                                config->SetLogLevel(LogLevel::kWarning);
                                return true;
                            }

                            if (set_command->value_str() == "error") {
                                SetLogLevel(LogLevel::kError);
                                config->SetLogLevel(LogLevel::kError);
                                return true;
                            }

                            if (set_command->value_str() == "critical") {
                                SetLogLevel(LogLevel::kCritical);
                                config->SetLogLevel(LogLevel::kCritical);
                                return true;
                            }

                            Status status = Status::SetInvalidVarValue("log level", "trace, debug, info, warning, error, critical");
                            RecoverableError(status);
                            break;
                        }
                        case GlobalOptionIndex::kRecordRunningQuery: {
                            if (set_command->value_type() != SetVarType::kBool) {
                                Status status = Status::DataTypeMismatch("Boolean", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            bool flag = set_command->value_bool();
                            if (config->RecordRunningQuery() && !flag) {
                                // turn off the query recording and clean all query record.
                                query_context->session_manager()->ClearQueryRecord();
                            }
                            config->SetRecordRunningQuery(flag);
                            break;
                        }
                        case GlobalOptionIndex::kCleanupInterval: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i64 interval = set_command->value_int();
                            if (interval < 0) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set cleanup interval: {}", interval));
                                RecoverableError(status);
                            }
                            query_context->storage()->periodic_trigger_thread()->cleanup_trigger_->UpdateInternal(interval);
                            config->SetCleanupInterval(interval);
                            break;
                        }
                        case GlobalOptionIndex::kFullCheckpointInterval: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i64 interval = set_command->value_int();
                            if (interval < 0) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set full checkpoint interval: {}", interval));
                                RecoverableError(status);
                            }
                            query_context->storage()->periodic_trigger_thread()->full_checkpoint_trigger_->UpdateInternal(interval);
                            config->SetFullCheckpointInterval(interval);
                            break;
                        }
                        case GlobalOptionIndex::kDeltaCheckpointInterval: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i64 interval = set_command->value_int();
                            if (interval < 0) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set delta checkpoint interval: {}", interval));
                                RecoverableError(status);
                            }
                            query_context->storage()->periodic_trigger_thread()->delta_checkpoint_trigger_->UpdateInternal(interval);
                            config->SetDeltaCheckpointInterval(interval);
                            break;
                        }
                        case GlobalOptionIndex::kCompactInterval: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i64 interval = set_command->value_int();
                            if (interval < 0) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set compact segment interval: {}", interval));
                                RecoverableError(status);
                            }
                            query_context->storage()->periodic_trigger_thread()->compact_segment_trigger_->UpdateInternal(interval);
                            config->SetCompactInterval(interval);
                            break;
                        }
                        case GlobalOptionIndex::kOptimizeIndexInterval: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i64 interval = set_command->value_int();
                            if (interval < 0) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set optimize interval interval: {}", interval));
                                RecoverableError(status);
                            }
                            query_context->storage()->periodic_trigger_thread()->optimize_index_trigger_->UpdateInternal(interval);
                            config->SetOptimizeInterval(interval);
                            break;
                        }
                        case GlobalOptionIndex::kTimeZone:{
                            if (set_command->value_type() != SetVarType::kString) {
                                Status status = Status::DataTypeMismatch("String", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            String tz;
                            i32 tz_bias = 0;
                            Config::ParseTimeZoneStr(set_command->value_str(), tz, tz_bias);
                            if (tz_bias < -12 || tz_bias > 12) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set time zone bias: {}", tz_bias));
                                RecoverableError(status);
                            }
                            if (tz == "UTC" || tz == "GMT") {
                                config->SetTimeZone(set_command->value_str());
                                config->SetTimeZoneBias(tz_bias);
                                return true;
                            }
                            break;
                        }
                        case GlobalOptionIndex::kTimeZoneBias: {
                            if (set_command->value_type() != SetVarType::kInteger) {
                                Status status = Status::DataTypeMismatch("Integer", set_command->value_type_str());
                                RecoverableError(status);
                            }
                            i64 bias = set_command->value_int();
                            if (bias < -12 || bias > 12) {
                                Status status = Status::InvalidCommand(fmt::format("Attempt to set time zone bias: {}", bias));
                                RecoverableError(status);
                            }
                            config->SetTimeZoneBias(bias);
                            break;
                        }
                        case GlobalOptionIndex::kInvalid: {
                            Status status = Status::InvalidCommand(fmt::format("Unknown config: {}", set_command->var_name()));
                            RecoverableError(status);
                            break;
                        }
                        default: {
                            Status status = Status::InvalidCommand(fmt::format("Config {} is read-only", set_command->var_name()));
                            RecoverableError(status);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    Status status = Status::InvalidCommand("Invalid set command scope, neither session nor global");
                    RecoverableError(status);
                }
            }
            break;
        }
        case CommandType::kExport: {
            ExportCmd *export_command = (ExportCmd *)(command_info_.get());

            auto profiler_record = InfinityContext::instance().storage()->catalog()->GetProfileRecord(export_command->file_no());
            if (profiler_record == nullptr) {
                Status status = Status::DataNotExist(fmt::format("The record does not exist: {}", export_command->file_no()));
                RecoverableError(status);
            }
            FileWriter file_writer(export_command->file_name(), 128);

            auto json = QueryProfiler::Serialize(profiler_record).dump();
            file_writer.Write(json.c_str(), json.size());
            file_writer.Flush();
            break;
        }
        case CommandType::kCheckTable: {
            break;
        }
        case CommandType::kLockTable: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
                operator_state->SetComplete();
                return true;
            }

            [[maybe_unused]] auto *lock_table_command = static_cast<LockCmd *>(command_info_.get());
            auto *txn = query_context->GetTxn();
            Status status = txn->LockTable(lock_table_command->db_name(), lock_table_command->table_name());
            if (!status.ok()) {
                RecoverableError(status);
            }
            break;
        }
        case CommandType::kUnlockTable: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
                operator_state->SetComplete();
                return true;
            }

            [[maybe_unused]] auto *unlock_table_command = static_cast<UnlockCmd *>(command_info_.get());
            auto *txn = query_context->GetTxn();
            Status status = txn->UnLockTable(unlock_table_command->db_name(), unlock_table_command->table_name());
            if (!status.ok()) {
                RecoverableError(status);
            }
            break;
        }
        case CommandType::kCleanup: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
                operator_state->SetComplete();
                return true;
            }

            auto *bg_process = query_context->storage()->bg_processor();
            {
                Txn *txn = query_context->GetTxn();
                auto checkpoint_task = MakeShared<ForceCheckpointTask>(txn, false /*is_full_checkpoint*/);
                bg_process->Submit(checkpoint_task);
                checkpoint_task->Wait();
            }
            {
                CleanupPeriodicTrigger *cleanup_trigger = query_context->storage()->bg_processor()->cleanup_trigger();
                SharedPtr<CleanupTask> cleanup_task = cleanup_trigger->CreateCleanupTask();
                if (cleanup_task.get() != nullptr) {
                    bg_process->Submit(cleanup_task);
                    cleanup_task->Wait();
                } else {
                    LOG_DEBUG("Skip cleanup");
                }
            }
            break;
        }
        case CommandType::kTestCommand: {
            auto *test_command = static_cast<TestCmd *>(command_info_.get());
            LOG_INFO(fmt::format("Execute test command: {}", test_command->command_content()));
            if (test_command->command_content() == "stuck dump by line bg_task for 3 second") {
                auto *compact_processor = query_context->storage()->compaction_processor();
                compact_processor->AddTestCommand(BGTaskType::kTestCommand, "stuck for 3 seconds");
            } else if (test_command->command_content() == "delta checkpoint") {
                LOG_INFO(fmt::format("test command: delta checkpoint"));
            } else {
                LOG_INFO(fmt::format("test command: other"));
            }
            break;
        }
        case CommandType::kSnapshot: {
            SnapshotCmd *snapshot_cmd = static_cast<SnapshotCmd *>(command_info_.get());
            LOG_INFO(fmt::format("Execute snapshot command"));
            SnapshotOp snapshot_operation = snapshot_cmd->operation();
            SnapshotScope snapshot_scope = snapshot_cmd->scope();
            const String &snapshot_name = snapshot_cmd->name();

            {
                // Full checkpoint
                Txn *txn = query_context->GetTxn();
                auto checkpoint_task = MakeShared<ForceCheckpointTask>(txn, true);
                query_context->storage()->bg_processor()->Submit(checkpoint_task);
                checkpoint_task->Wait();
            }

            switch (snapshot_operation) {
                case SnapshotOp::kCreate: {
                    LOG_INFO(fmt::format("Execute snapshot create"));

                    switch (snapshot_scope) {
                        case SnapshotScope::kSystem: {
                            LOG_INFO(fmt::format("Execute snapshot system"));
                            break;
                        }
                        case SnapshotScope::kDatabase: {
                            LOG_INFO(fmt::format("Execute snapshot database"));
                            break;
                        }
                        case SnapshotScope::kTable: {
                            const String &table_name = snapshot_cmd->object_name();
                            Status snapshot_status = Snapshot::CreateTableSnapshot(query_context, snapshot_name, table_name);
                            if (!snapshot_status.ok()) {
                                RecoverableError(snapshot_status);
                            }
                            LOG_INFO(fmt::format("Execute snapshot table"));
                            break;
                        }
                        case SnapshotScope::kIgnore: {
                            LOG_INFO(fmt::format("Execute snapshot ignore"));
                            break;
                        }
                        default: {
                            String error_message = "Invalid snapshot scope";
                            UnrecoverableError(error_message);
                            break;
                        }
                    }
                    break;
                }
                case SnapshotOp::kDrop: {
                    LOG_INFO(fmt::format("Execute snapshot drop"));
                    Status snapshot_status = Snapshot::DropSnapshot(query_context, snapshot_name);
                    if (!snapshot_status.ok()) {
                        RecoverableError(snapshot_status);
                    }
                    break;
                }
                case SnapshotOp::kRestore: {
                    LOG_INFO(fmt::format("Execute snapshot restore"));
                    switch (snapshot_scope) {
                        case SnapshotScope::kSystem: {
                            LOG_INFO(fmt::format("Execute snapshot system restore"));
                            break;
                        }
                        case SnapshotScope::kDatabase: {
                            LOG_INFO(fmt::format("Execute snapshot database restore"));
                            break;
                        }
                        case SnapshotScope::kTable: {
                            Status snapshot_status = Snapshot::RestoreTableSnapshot(query_context, snapshot_name);
                            if (!snapshot_status.ok()) {
                                RecoverableError(snapshot_status);
                            }
                            LOG_INFO(fmt::format("Execute snapshot table restore"));
                            break;
                        }
                        case SnapshotScope::kIgnore: {
                            LOG_INFO(fmt::format("Execute snapshot ignore restore"));
                            break;
                        }
                        default: {
                            String error_message = "Invalid snapshot scope";
                            UnrecoverableError(error_message);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    String error_message = "Invalid snapshot operation type";
                    UnrecoverableError(error_message);
                    break;
                }
            }

            break;
        }
        default: {
            String error_message = fmt::format("Invalid command type: {}", command_info_->ToString());
            UnrecoverableError(error_message);
        }
    }
    return true;
}

} // namespace infinity