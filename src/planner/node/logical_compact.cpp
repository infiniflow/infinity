module;

#include <sstream>

module logical_compact;

import stl;
import parser;
import column_binding;

namespace infinity {

Vector<ColumnBinding> LogicalCompact::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalCompact::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCompact::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalCompact::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> "
       << "Compact " << *table_entry_->GetDBName() << '.' << *table_entry_->GetTableName();
    return ss.str();
}

} // namespace infinity