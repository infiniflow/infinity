//
// Created by JinHai on 2022/9/26.
//

module;

import std;

import stl;
import column_binding;
import base_expression;
import parser;

module logical_project;

namespace infinity {

Vector<ColumnBinding> LogicalProject::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    SizeT expression_count = expressions_.size();
    result.reserve(expression_count);
    for (SizeT i = 0; i < expression_count; ++i) {
        result.emplace_back(table_index_, i);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalProject::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SizeT expression_count = expressions_.size();
    result->reserve(expression_count);
    for (SizeT i = 0; i < expression_count; ++i) {
        result->emplace_back(expressions_[i]->Name());
    }

    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalProject::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = MakeShared<Vector<SharedPtr<DataType>>>();
    SizeT column_count = expressions_.size();
    result->reserve(column_count);
    for (SizeT i = 0; i < column_count; ++i) {
        result->emplace_back(MakeShared<DataType>(expressions_[i]->Type()));
    }
    return result;
}

String LogicalProject::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Project on: ";
    SizeT expression_count = expressions_.size();
    for (SizeT i = 0; i < expression_count - 1; ++i) {
        ss << expressions_[i]->Name() << ", ";
    }
    ss << expressions_.back()->Name();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
