//
// Created by JinHai on 2022/7/23.
//

#include "logical_insert.h"
#include "planner/column_binding.h"
#include "common/types/data_type.h"
#include "storage/meta/entry/table_collection_entry.h"
#include <sstream>

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
