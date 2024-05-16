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
            switch(export_cmd_info->export_type()) {
                case ExportType::kProfileRecord: {
                    ss << "Profile Record: " << export_cmd_info->file_no();
                }
            }
            break;
        }
        case CommandType::kSet: {
            SetCmd* set_cmd_info = (SetCmd*)(command_info_.get());
            ss << String(space, ' ') << arrow_str;
            if(set_cmd_info->scope() == SetScope::kSession) {
                ss << "Set session variable: ";
            } else {
                ss << "Set global variable: ";
            }
            switch(set_cmd_info->value_type()) {
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
                    UnrecoverableError("Invalid variable type.");
                }
            }
            break;
        }
        case CommandType::kCheckTable: {
            CheckTable *check_table_info = (CheckTable *)(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Check table: " << check_table_info->table_name();
            break;
        }
        case CommandType::kCompactTable: {
            CompactTable *compact_table_info = (CompactTable *)(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Compact table: " << compact_table_info->schema_name_ << " " << compact_table_info->table_name_;
            break;
        }
        case CommandType::kInvalid: {
            UnrecoverableError("Invalid command type.");
        }
    }

    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
