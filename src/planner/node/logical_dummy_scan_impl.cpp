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

module infinity_core:logical_dummy_scan.impl;

import :logical_dummy_scan;
import :column_binding;
import :logical_dummy_scan;

import std;

import data_type;
// import internal_types;

namespace infinity {

std::vector<ColumnBinding> LogicalDummyScan::GetColumnBindings() const { return {}; }

std::shared_ptr<std::vector<std::string>> LogicalDummyScan::GetOutputNames() const { return std::make_shared<std::vector<std::string>>(); }

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalDummyScan::GetOutputTypes() const { return std::make_shared<std::vector<std::shared_ptr<DataType>>>(); }

std::string LogicalDummyScan::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << "-> "
       << "DummyScan: " << this->table_alias_ << "(" << this->table_index_ << ")";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
