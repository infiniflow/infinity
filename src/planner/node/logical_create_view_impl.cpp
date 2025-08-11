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

#include <sstream>

module infinity_core:logical_create_view.impl;

import :logical_create_view;

import :stl;
import :column_binding;
import logical_type;
import internal_types;
import :logical_create_view;
import create_view_info;
import data_type;

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

const SharedPtr<CreateViewInfo> LogicalCreateView::create_view_info() const { return create_view_info_; }

String LogicalCreateView::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Create View: " << create_view_info_->schema_name_ << "." << create_view_info_->view_name_;
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
