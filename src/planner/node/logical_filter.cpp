//
// Created by JinHai on 2022/8/9.
//

module;

import std;

import stl;
import parser;
import column_binding;

module logical_filter;

namespace infinity {

Vector<ColumnBinding> LogicalFilter::GetColumnBindings() const { return left_node_->GetColumnBindings(); }

SharedPtr<Vector<String>> LogicalFilter::GetOutputNames() const { return left_node_->GetOutputNames(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalFilter::GetOutputTypes() const { return left_node_->GetOutputTypes(); }

String LogicalFilter::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Logical Filter: " << expression_->Name();
    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity
