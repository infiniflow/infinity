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

module logical_update;

namespace infinity {

Vector<ColumnBinding> LogicalUpdate::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalUpdate::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalUpdate::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalUpdate::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "UPDATE ";
    const DBEntry *db_entry = TableCollectionEntry::GetDBEntry(table_entry_ptr_);
    ss << *db_entry->db_name_ << "." << *table_entry_ptr_->table_collection_name_;
    ss << " SET";
    for (SizeT i = 0; i < update_columns_.size(); i++) {
        if (i > 0)
            ss << ", ";
        ss << " ";
        ss << table_entry_ptr_->columns_[update_columns_[i].first]->name_;
        ss << " = " << update_columns_[i].second->Name();
    }
    return ss.str();
}

} // namespace infinity
