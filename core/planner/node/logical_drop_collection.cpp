//
// Created by jinhai on 23-3-6.
//

module;

#include <sstream>

import stl;
import column_binding;
import logical_node_type;
import parser;

module logical_drop_collection;

namespace infinity {

Vector<ColumnBinding> LogicalDropCollection::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalDropCollection::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalDropCollection::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalDropCollection::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Drop Collection: " << *schema_name_ << "." << *collection_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
