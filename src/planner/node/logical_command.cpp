//
// Created by jinhai on 23-10-31.
//

module;

import std;

import stl;
import column_binding;
import logical_node_type;
import parser;
import infinity_assert;
import infinity_exception;

module logical_command;

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

    switch(command_info_->type()) {
        case CommandType::kUse: {
            UseCmd* use_cmd_info = (UseCmd*)(command_info_.get());
            ss << String(space, ' ') << arrow_str << "Use table: " << use_cmd_info->db_name();
        }
        case CommandType::kInvalid: {
            Error<PlannerException>("Invalid command type.", __FILE_NAME__, __LINE__);
        }
    }

    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
