// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

module infinity_core:logical_project.impl;

import :logical_project;
import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;

import std;
import third_party;

import internal_types;
import data_type;

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
