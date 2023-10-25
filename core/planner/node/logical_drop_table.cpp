//
// Created by JinHai on 2022/7/23.
//

module;

#include <sstream>

import stl;
import column_binding;
import logical_node_type;
import parser;

module logical_drop_table;

namespace infinity {

Vector<ColumnBinding> LogicalDropTable::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalDropTable::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalDropTable::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalDropTable::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Drop Table: " << *schema_name_ << "." << *table_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
