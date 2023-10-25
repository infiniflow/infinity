//
// Created by JinHai on 2022/10/25.
//

module;

#include <sstream>

import stl;
import column_binding;
import parser;

module logical_dummy_scan;

namespace infinity {

Vector<ColumnBinding> LogicalDummyScan::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalDummyScan::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalDummyScan::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalDummyScan::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> "
       << "DummyScan: " << this->table_alias_ << "(" << this->table_index_ << ")";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
