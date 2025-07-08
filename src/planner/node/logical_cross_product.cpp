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

module infinity_core;

import :stl;
import :column_binding;
import :logical_node_type;
import :logical_node;
import :logical_cross_product;
import internal_types;
import data_type;

namespace infinity {

LogicalCrossProduct::LogicalCrossProduct(u64 node_id, String alias, const SharedPtr<LogicalNode> &left, const SharedPtr<LogicalNode> &right)
    : LogicalNode(node_id, LogicalNodeType::kCrossProduct), alias_(std::move(alias)) {
    this->set_left_node(left);
    this->set_right_node(right);
}

Vector<ColumnBinding> LogicalCrossProduct::GetColumnBindings() const {
    Vector<ColumnBinding> left_binding = this->left_node_->GetColumnBindings();
    Vector<ColumnBinding> right_binding = this->right_node_->GetColumnBindings();
    left_binding.insert(left_binding.end(), right_binding.begin(), right_binding.end());
    return left_binding;
}

SharedPtr<Vector<String>> LogicalCrossProduct::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SharedPtr<Vector<String>> left_output_names = left_node_->GetOutputNames();
    SharedPtr<Vector<String>> right_output_names = right_node_->GetOutputNames();

    SizeT left_name_count = left_output_names->size();
    SizeT right_name_count = right_output_names->size();

    result->reserve(left_name_count + right_name_count);
    for (SizeT idx = 0; idx < left_name_count; ++idx) {
        const auto &name_str = left_output_names->at(idx);
        result->emplace_back(name_str);
    }

    for (SizeT idx = 0; idx < right_name_count; ++idx) {
        const auto &name_str = right_output_names->at(idx);
        result->emplace_back(name_str);
    }

    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCrossProduct::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = MakeShared<Vector<SharedPtr<DataType>>>();
    SharedPtr<Vector<SharedPtr<DataType>>> left_output_types = left_node_->GetOutputTypes();
    SharedPtr<Vector<SharedPtr<DataType>>> right_output_types = right_node_->GetOutputTypes();

    SizeT left_type_count = left_output_types->size();
    SizeT right_type_count = right_output_types->size();
    result->reserve(left_type_count + right_type_count);

    for (SizeT idx = 0; idx < left_type_count; ++idx) {
        const auto &name_str = left_output_types->at(idx);
        result->emplace_back(name_str);
    }

    for (SizeT idx = 0; idx < right_type_count; ++idx) {
        const auto &name_str = right_output_types->at(idx);
        result->emplace_back(name_str);
    }

    return result;
}

String LogicalCrossProduct::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Cross Product: ";
    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity
