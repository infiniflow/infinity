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

module infinity_core:logical_filter.impl;

import :logical_filter;

import :stl;
import :column_binding;
import :logical_node;
import internal_types;
import data_type;

namespace infinity {

Vector<ColumnBinding> LogicalFilter::GetColumnBindings() const { return LogicalCommonFunctionUsingLoadMeta::GetColumnBindings(*this); }

SharedPtr<Vector<String>> LogicalFilter::GetOutputNames() const { return LogicalCommonFunctionUsingLoadMeta::GetOutputNames(*this); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalFilter::GetOutputTypes() const { return LogicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this); }

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
