//
// Created by jinhai on 23-3-6.
//

module;

import stl;
import column_binding;
import parser;

module logical_create_view;

namespace infinity {

Vector<ColumnBinding> LogicalCreateView::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalCreateView::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCreateView::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalCreateView::ToString(i64 &space) const {
//    std::stringstream ss;
//    String arrow_str;
//    if (space > 3) {
//        space -= 4;
//        arrow_str = "->  ";
//    }
//    ss << String(space, ' ') << arrow_str << "Create View: " << create_view_info_->schema_name_ << "." << create_view_info_->view_name_;
//    space += arrow_str.size();
//
//    return ss.str();
}

} // namespace infinity
