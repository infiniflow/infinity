//
// Created by JinHai on 2022/7/23.
//

module;

#include <sstream>

import stl;
import parser;
import logical_node;
import logical_node_type;
import column_binding;
import base_table_ref;
import column_binding;
import logical_node_type;
import table_collection_entry;
import db_entry;

module logical_delete;

namespace infinity {

Vector<ColumnBinding> LogicalDelete::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalDelete::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalDelete::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalDelete::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "DELETE FROM ";
    const DBEntry *db_entry = TableCollectionEntry::GetDBEntry(table_entry_ptr_);
    ss << *db_entry->db_name_ << "." << *table_entry_ptr_->table_collection_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
