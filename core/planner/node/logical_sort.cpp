//
// Created by JinHai on 2022/9/26.
//

module;

import stl;
import column_binding;
import parser;
import base_expression;

module logical_sort;

namespace infinity {

Vector<ColumnBinding> LogicalSort::GetColumnBindings() const { return left_node_->GetColumnBindings(); }

SharedPtr<Vector<String>> LogicalSort::GetOutputNames() const { return left_node_->GetOutputNames(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalSort::GetOutputTypes() const { return left_node_->GetOutputTypes(); }

String LogicalSort::ToString(i64 &space) const {

//    std::stringstream ss;
//    String arrow_str;
//    if (space > 3) {
//        space -= 4;
//        arrow_str = "->  ";
//    }
//    ss << String(space, ' ') << arrow_str << "Order by: ";
//    SizeT expression_count = expressions_.size();
//    for (SizeT i = 0; i < expression_count - 1; ++i) {
//        ss << expressions_[i]->Name() << " " << infinity::ToString(order_by_types_[i]) << ", ";
//    }
//    ss << expressions_.back()->Name() << " " << infinity::ToString(order_by_types_.back());
//    space += arrow_str.size();
//
//    return ss.str();
}

} // namespace infinity
