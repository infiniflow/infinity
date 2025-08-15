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

module infinity_core:logical_join.impl;

import :logical_join;
import :stl;
import :column_binding;
import :logical_node_type;
import :base_expression;

import std;

import internal_types;
import join_reference;
import data_type;

namespace infinity {

LogicalJoin::LogicalJoin(u64 node_id,
                         JoinType join_type,
                         std::string alias,
                         std::vector<std::shared_ptr<BaseExpression>> conditions,
                         const std::shared_ptr<LogicalNode> &left,
                         const std::shared_ptr<LogicalNode> &right)
    : LogicalNode(node_id, LogicalNodeType::kJoin), alias_(std::move(alias)), join_type_(join_type), conditions_(std::move(conditions)) {
    this->set_left_node(left);
    this->set_right_node(right);
}

std::vector<ColumnBinding> LogicalJoin::GetColumnBindings() const {
    std::vector<ColumnBinding> result_binding;
    if (join_type_ == JoinType::kMark) {
        result_binding.emplace_back(mark_index_, 0);
    }
    std::vector<ColumnBinding> left_binding = this->left_node_->GetColumnBindings();
    std::vector<ColumnBinding> right_binding = this->right_node_->GetColumnBindings();
    result_binding.insert(result_binding.end(), left_binding.begin(), left_binding.end());
    result_binding.insert(result_binding.end(), right_binding.begin(), right_binding.end());
    return result_binding;
}

std::shared_ptr<std::vector<std::string>> LogicalJoin::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    std::shared_ptr<std::vector<std::string>> left_output_names = left_node_->GetOutputNames();
    std::shared_ptr<std::vector<std::string>> right_output_names = right_node_->GetOutputNames();
    result->reserve(left_output_names->size() + right_output_names->size());
    for (auto &name_str : *left_output_names) {
        result->emplace_back(name_str);
    }

    for (auto &name_str : *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalJoin::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> left_output_names = left_node_->GetOutputTypes();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> right_output_names = right_node_->GetOutputTypes();
    result->reserve(left_output_names->size() + right_output_names->size());
    for (auto &name_str : *left_output_names) {
        result->emplace_back(name_str);
    }

    for (auto &name_str : *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
}

std::string LogicalJoin::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << JoinReference::ToString(join_type_) << " on ";
    for (auto &condition : conditions_) {
        ss << condition->Name() << " ";
    }
    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity
