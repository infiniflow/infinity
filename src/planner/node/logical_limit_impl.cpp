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

module infinity_core:logical_limit.impl;

import :logical_limit;
import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;

import std;

import internal_types;
import data_type;

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
    if (offset_expression_.get() != nullptr) {
        ss << ", offset: " << offset_expression_->Name();
    }
    ss << ")";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
