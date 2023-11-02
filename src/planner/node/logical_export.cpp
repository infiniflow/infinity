//
// Created by jinhai on 23-8-29.
//

module;

import std;

import stl;
import column_binding;
import parser;

module logical_export;

namespace infinity {

Vector<ColumnBinding> LogicalExport::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalExport::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalExport::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalExport::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << String(space, ' ') << "-> "
       << "Export from: " << file_path_;

    switch (file_type_) {
        case CopyFileType::kCSV: {
            ss << "(CSV) ";
            break;
        }
        case CopyFileType::kJSON: {
            ss << "(JSON) ";
            break;
        }
        case CopyFileType::kFVECS: {
            ss << "(FVECS) ";
            break;
        }
    }
    ss << "to " << schema_name_ << '.' << table_name_;

    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
