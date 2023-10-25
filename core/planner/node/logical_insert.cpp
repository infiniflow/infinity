//
// Created by JinHai on 2022/7/23.
//

module;

#include <sstream>

import stl;
import column_binding;
import parser;
import table_collection_entry;

module logical_insert;

namespace infinity {

Vector<ColumnBinding> LogicalInsert::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalInsert::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalInsert::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalInsert::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Insert Table: " << *table_collection_entry_->table_collection_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
