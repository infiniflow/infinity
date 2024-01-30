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
    auto *table_entry = table_ref_->table_entry_ptr_;
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> "
       << "Compact " << *table_ref_->schema_name() << '.' << *table_ref_->table_name();
    return ss.str();
}

} // namespace infinity