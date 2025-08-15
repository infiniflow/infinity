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

module infinity_core:logical_project.impl;

import :logical_project;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_expression;

import std;

import internal_types;
import data_type;

namespace infinity {

std::vector<ColumnBinding> LogicalProject::GetColumnBindings() const {
    std::vector<ColumnBinding> result;
    size_t expression_count = expressions_.size();
    result.reserve(expression_count);
    for (size_t i = 0; i < expression_count; ++i) {
        result.emplace_back(table_index_, i);
    }
    return result;
}

std::shared_ptr<std::vector<std::string>> LogicalProject::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    size_t expression_count = expressions_.size();
    result->reserve(expression_count);
    for (size_t i = 0; i < expression_count; ++i) {
        result->emplace_back(expressions_[i]->Name());
    }

    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalProject::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    size_t column_count = expressions_.size();
    result->reserve(column_count);
    for (size_t i = 0; i < column_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(expressions_[i]->Type()));
    }
    return result;
}

std::string LogicalProject::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Project on: ";
    size_t expression_count = expressions_.size();
    for (size_t i = 0; i < expression_count - 1; ++i) {
        ss << expressions_[i]->Name() << ", ";
    }
    ss << expressions_.back()->Name();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
