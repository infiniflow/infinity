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

module infinity_core:logical_cross_product.impl;

import :logical_cross_product;
import :column_binding;
import :logical_node_type;
import :logical_node;
import :logical_cross_product;

import std;

import internal_types;
import data_type;

namespace infinity {

LogicalCrossProduct::LogicalCrossProduct(u64 node_id,
                                         std::string alias,
                                         const std::shared_ptr<LogicalNode> &left,
                                         const std::shared_ptr<LogicalNode> &right)
    : LogicalNode(node_id, LogicalNodeType::kCrossProduct), alias_(std::move(alias)) {
    this->set_left_node(left);
    this->set_right_node(right);
}

std::vector<ColumnBinding> LogicalCrossProduct::GetColumnBindings() const {
    std::vector<ColumnBinding> left_binding = this->left_node_->GetColumnBindings();
    std::vector<ColumnBinding> right_binding = this->right_node_->GetColumnBindings();
    left_binding.insert(left_binding.end(), right_binding.begin(), right_binding.end());
    return left_binding;
}

std::shared_ptr<std::vector<std::string>> LogicalCrossProduct::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    std::shared_ptr<std::vector<std::string>> left_output_names = left_node_->GetOutputNames();
    std::shared_ptr<std::vector<std::string>> right_output_names = right_node_->GetOutputNames();

    size_t left_name_count = left_output_names->size();
    size_t right_name_count = right_output_names->size();

    result->reserve(left_name_count + right_name_count);
    for (size_t idx = 0; idx < left_name_count; ++idx) {
        const auto &name_str = left_output_names->at(idx);
        result->emplace_back(name_str);
    }

    for (size_t idx = 0; idx < right_name_count; ++idx) {
        const auto &name_str = right_output_names->at(idx);
        result->emplace_back(name_str);
    }

    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalCrossProduct::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> left_output_types = left_node_->GetOutputTypes();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> right_output_types = right_node_->GetOutputTypes();

    size_t left_type_count = left_output_types->size();
    size_t right_type_count = right_output_types->size();
    result->reserve(left_type_count + right_type_count);

    for (size_t idx = 0; idx < left_type_count; ++idx) {
        const auto &name_str = left_output_types->at(idx);
        result->emplace_back(name_str);
    }

    for (size_t idx = 0; idx < right_type_count; ++idx) {
        const auto &name_str = right_output_types->at(idx);
        result->emplace_back(name_str);
    }

    return result;
}

std::string LogicalCrossProduct::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Cross Product: ";
    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity
