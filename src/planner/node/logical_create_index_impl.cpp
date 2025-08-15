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

module infinity_core:logical_create_index.impl;

import :logical_create_index;
import :stl;
import :column_binding;
import :base_expression;
import :index_base;

import std;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

std::vector<ColumnBinding> LogicalCreateIndex::GetColumnBindings() const { return {}; };

std::shared_ptr<std::vector<std::string>> LogicalCreateIndex::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    result->emplace_back("OK");
    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalCreateIndex::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result_type = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    result_type->emplace_back(std::make_shared<DataType>(LogicalType::kInteger));
    return result_type;
}

std::string LogicalCreateIndex::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Create Table: " << *base_table_ref_->table_name() << "." << index_definition_->ToString();
    space += arrow_str.size();

    return ss.str();
}
} // namespace infinity
