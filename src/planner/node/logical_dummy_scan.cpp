//
// Created by JinHai on 2022/10/25.
//

#include "logical_dummy_scan.h"
#include "planner/column_binding.h"
#include "common/types/data_type.h"
#include <sstream>

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
