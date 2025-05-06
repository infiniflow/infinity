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

#include <sstream>

module logical_command;

import stl;
import column_binding;
import logical_node_type;

import logical_type;
import infinity_exception;
import internal_types;
import logger;

namespace infinity {

Vector<ColumnBinding> LogicalCommand::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalCommand::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCommand::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalCommand::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    switch (command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_cmd_info = (UseCmd *)(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Use table: " << use_cmd_info->db_name();
            break;
        }
        case CommandType::kExport: {
            ExportCmd *export_cmd_info = (ExportCmd *)(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Export ";
            switch (export_cmd_info->export_type()) {
                case ExportType::kProfileRecord: {
                    ss << "Profile Record: " << export_cmd_info->file_no();
                }
            }
            break;
        }
        case CommandType::kSet: {
            SetCmd *set_cmd_info = (SetCmd *)(command_info_.get());
            ss << String(space, ' ') << arrow_str;
            if (set_cmd_info->scope() == SetScope::kSession) {
                ss << "Set session variable: ";
            } else {
                ss << "Set global variable: ";
            }
            switch (set_cmd_info->value_type()) {
                case SetVarType::kBool: {
                    ss << set_cmd_info->var_name() << " = " << set_cmd_info->value_bool();
                    break;
                }
                case SetVarType::kInteger: {
                    ss << set_cmd_info->var_name() << " = " << set_cmd_info->value_int();
                    break;
                }
                case SetVarType::kDouble: {
                    ss << set_cmd_info->var_name() << " = " << set_cmd_info->value_double();
                    break;
                }
                case SetVarType::kString: {
                    ss << set_cmd_info->var_name() << " = " << set_cmd_info->value_str();
                    break;
                }
                case SetVarType::kInvalid: {
                    String error_message = "Invalid variable type.";
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        case CommandType::kCheckTable: {
            CheckTable *check_table_info = (CheckTable *)(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Check table: " << check_table_info->table_name();
            break;
        }
        case CommandType::kLockTable: {
            auto *lock_table_info = static_cast<LockCmd *>(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Lock table: " << lock_table_info->db_name() << "." << lock_table_info->table_name();
            break;
        }
        case CommandType::kUnlockTable: {
            auto *unlock_table_info = static_cast<UnlockCmd *>(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Lock table: " << unlock_table_info->db_name() << "." << unlock_table_info->table_name();
            break;
        }
        case CommandType::kCleanup: {
            ss << String(space, ' ') << arrow_str << "Cleanup";
            break;
        }
        case CommandType::kDumpIndex: {
            ss << String(space, ' ') << arrow_str << "Dump index: ";
            auto *dump_index_info = static_cast<DumpIndexCmd *>(command_info_.get());
            ss << dump_index_info->db_name() << "." << dump_index_info->table_name() << "." << dump_index_info->index_name();
            break;
        }
        case CommandType::kTestCommand: {
            auto *test_command_info = static_cast<TestCmd *>(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Test command: " << test_command_info->command_content();
            break;
        }
        case CommandType::kSnapshot: {
            auto *snapshot_info = static_cast<SnapshotCmd *>(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Snapshot command: ";
            switch(snapshot_info->operation()) {
                case SnapshotOp::kCreate: {
                    ss << "CREATE ";
                    break;
                }
                case SnapshotOp::kDrop: {
                    ss << "DROP ";
                    break;
                }
                case SnapshotOp::kRestore: {
                    ss << "RESTORE ";
                    break;
                }
                case SnapshotOp::kInvalid: {
                    String error_message = "Invalid snapshot operation type.";
                    UnrecoverableError(error_message);
                }
            }

            switch(snapshot_info->scope()) {
                case SnapshotScope::kSystem: {
                    ss << "SYSTEM ";
                    break;
                }
                case SnapshotScope::kDatabase: {
                    ss << "DATABASE ";
                    break;
                }
                case SnapshotScope::kTable: {
                    ss << "TABLE ";
                    break;
                }
                case SnapshotScope::kIgnore: {
                    break;
                }
                case SnapshotScope::kInvalid: {
                    String error_message = "Invalid snapshot scope.";
                    UnrecoverableError(error_message);
                }
            }

            ss << snapshot_info->name();
            break;
        }
        case CommandType::kInvalid: {
            String error_message = "Invalid command type.";
            UnrecoverableError(error_message);
        }
    }

    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
