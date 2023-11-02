//
// Created by JinHai on 2022/9/26.
//

module;

#include <memory>

import std;
import stl;
import column_binding;
import parser;
import base_expression;

module logical_limit;

namespace infinity {

Vector<ColumnBinding> LogicalLimit::GetColumnBindings() const { return left_node_->GetColumnBindings(); }

SharedPtr<Vector<String>> LogicalLimit::GetOutputNames() const { return left_node_->GetOutputNames(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalLimit::GetOutputTypes() const { return left_node_->GetOutputTypes(); }

String LogicalLimit::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << String(space, ' ') << arrow_str << "Limit (limit: " << limit_expression_->Name();
    if (offset_expression_ != nullptr) {
        ss << ", offset: " << offset_expression_->Name();
    }
    ss << ")";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
