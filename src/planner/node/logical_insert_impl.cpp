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

module infinity_core:logical_insert.impl;

import :logical_insert;
import :column_binding;
import :logical_insert;

import std;

import internal_types;
import data_type;

namespace infinity {

std::vector<ColumnBinding> LogicalInsert::GetColumnBindings() const { return {}; }

std::shared_ptr<std::vector<std::string>> LogicalInsert::GetOutputNames() const { return std::make_shared<std::vector<std::string>>(); }

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalInsert::GetOutputTypes() const {
    return std::make_shared<std::vector<std::shared_ptr<DataType>>>();
}

std::string LogicalInsert::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    if (has_select_source()) {
        ss << std::string(space, ' ') << arrow_str << "Insert Table: " << *table_info_->table_name_ << " (from SELECT)";
    } else {
        ss << std::string(space, ' ') << arrow_str << "Insert Table: " << *table_info_->table_name_ << " (from VALUES)";
    }
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
